#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "assert.h"
#include "string.h"
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:
	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVertical = 40;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVertical();
	uint32_t GetNumBlockHorizontal();

	IndexSet GetMapChipSetByPosition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:
};
