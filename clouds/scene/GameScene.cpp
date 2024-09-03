#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete modelBlock_;
	delete debugCamera_;
	delete skyDome_;
	delete mapChipField_;
	delete cameraController_;

	for (std::vector<WorldTransform*>& worldTransferBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransferBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("cube/cube.jpg");
	modelSkyDome_ = Model::CreateFromOBJ("skydome", true);
	modelPlayer_ = Model::CreateFromOBJ("player", true);

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 5000;
	viewProjection_.Initialize();


	// skyDomeの生成
	skyDome_ = new skyDome();
	// skyDomeの初期化
	skyDome_->Initialize(modelSkyDome_, &viewProjection_);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");


	// 自キャラの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition);

	player_->SetMapChipField(mapChipField_);

	GenerateBlocks();
	modelBlock_ = new Model();

	// 画面縦幅
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(kWindowWidth, kWindowHeight);

	
	

	// カメラコントローラ
	cameraController_ = new CameraController;

	// カメラコントローラの初期化
	cameraController_->Initialize(&viewProjection_);

	// 移動範囲の指定
	CameraController::Rect area;
	area.left = 0.0f;
	area.right = 500.0f;
	area.top = 40.0f;
	area.bottom = 0.0f;
	cameraController_->SetMovableArea(area);

	// 追従対象をセット
	cameraController_->SetTarget(player_);
	// リセット
	cameraController_->Reset();
}

void GameScene::Update() {

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif  _DEBUG

	// カメラコントローラの更新
	cameraController_->Update();
	// 自キャラの更新
	player_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransferBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransferBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatrix();
		}
	}

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	// skyDomeの処理
	skyDome_->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 自キャラの描画
	player_->Draw();

	// モデルを連動
	modelBlock_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);

	// skyDomeの描画
	skyDome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransferBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransferBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			model_->Draw(*worldTransformBlock, viewProjection_);
			// モデルを連動
			modelBlock_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {

	// 要素数
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	uint32_t kNumBlockVertical = mapChipField_->GetNumBlockVertical();

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVertical);
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		// 一列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockHorizontal; ++i) {
		for (uint32_t j = 0; j < kNumBlockVertical; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[j][i] = worldTransform;
				worldTransformBlocks_[j][i]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
