#include "skydome.h"

skyDome::skyDome() {}

skyDome::~skyDome() {}

void skyDome::Initialize(Model* model, ViewProjection* viewProjection) {

	assert(model);
	model_ = model;

	//// skyDomeの生成
	// skyDome_ = new skyDome();
	//// skyDomeの初期化
	// skyDome_->Initialize();

	worldTransform_.Initialize();
	viewProjection_ = viewProjection;

	worldTransform_.scale_ = {1, 1, 1};
}

void skyDome::Update() {}

void skyDome::Draw() { model_->Draw(worldTransform_, *viewProjection_); }
