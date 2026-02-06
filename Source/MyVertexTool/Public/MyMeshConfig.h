#pragma once

constexpr uint32 k_LODMaxLevel = 3; // Lod最大级别
constexpr int32 k_TessQuadCount = 16; // Quad细分成个数 count x count
constexpr int32 k_QuadWorldScale = 5; // level0 对应几米 之后 x2 递增

// 注意这里只是最大缩放, 如果要计算位置 要乘以100  (1m == UE 100cm)
constexpr float k_MaxWorldScale = k_QuadWorldScale * (1 << k_LODMaxLevel);

constexpr int32 k_WorldMinHeight = -100; // 最低多少m
constexpr int32 k_WorldMaxHeight = 1000; // 最高多少m
constexpr int32 k_WorldHeightCenter = 0.5f * (k_WorldMinHeight + k_WorldMaxHeight);
constexpr int32 k_WorldHeightExtent = 0.5f * (k_WorldMaxHeight - k_WorldMinHeight);