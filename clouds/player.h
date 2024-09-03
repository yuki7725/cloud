
#pragma once
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "cassert"
#include <algorithm>

class MapChipField;

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool isCeiling = false; // 天井
	bool isGround = false;  // 地面
	bool isWall = false;    // 壁
	Vector3 movement;
};

// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上
	kNumCorner    // 要素数
};

class Player {
public:

	Player();
	~Player();

	void Initialize(Model* model, ViewProjection* viewProjection, Vector3& position);

	void Update();

	void Draw();

	WorldTransform& GetWorldTransform();

	const Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// マップ衝突判定関数
	void isCollision(CollisionMapInfo& info);
	void isCollisionTop(CollisionMapInfo& info);
	void isCollisionBottom(CollisionMapInfo& info);
	//void isCollisionBottom(CollisionMapInfo& info);
	

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// 判定結果を反映して移動させる
	void collisionResult(const CollisionMapInfo& info);

	// 天井に接触しているときの処理
	void isCellingCollision(const CollisionMapInfo& info);

	//接地状態(地面)の切り替え処理
	void isGroundCollision(const CollisionMapInfo& info);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	Vector3 velocity_ = {};

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 1.0f;

	enum class LRDirection {
		kRight,
		kLeft,
	};

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間(秒)
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度(下)
	static inline const float kGravityAcceleration = 0.1f;
	// 最大落下速度(下)
	static inline const float kLimitFallSpeed = 0.7f;
	// ジャンプ加速(上)
	static inline const float kJumpAcceleration = 0.5f;

	// 接地状態フラグ
	bool onGround_ = true;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 2.0f;

	//着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;
};