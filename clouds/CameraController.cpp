#define NOMINMAX
#include "CameraController.h"
#include "algorithm"
#include "cmath"
#include "iostream"
#include "player.h"

void CameraController::Initialize(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	Vector3 targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットからカメラの座標を計算
	cameraMarkPos_ = Add(Add(targetWorldTransform.translation_, targetOffset_), Multiply(kVelocityBias, targetVelocity));

	// 座標補間によりゆったり追従
	viewProjection_->translation_.x = std::lerp(viewProjection_->translation_.x, cameraMarkPos_.x, kInterpolationRate);
	viewProjection_->translation_.y = std::lerp(viewProjection_->translation_.y, cameraMarkPos_.y, kInterpolationRate);

	// 移動範囲制限
	viewProjection_->translation_.x = std::max(viewProjection_->translation_.x, margin.left);
	viewProjection_->translation_.x = std::min(viewProjection_->translation_.x, margin.right);
	viewProjection_->translation_.y = std::max(viewProjection_->translation_.y, margin.bottom);
	viewProjection_->translation_.y = std::min(viewProjection_->translation_.y, margin.top);

	// 行列を更新する
	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	// 追従対象とオフセットからカメラの座標を計算
	viewProjection_->translation_ = Add(targetWorldTransform.translation_, targetOffset_);
}
