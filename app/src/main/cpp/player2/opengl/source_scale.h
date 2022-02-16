//
// Created by richard on 2022/2/16.
//
#pragma once

#include "opengl_render.h"

namespace player {

    float*  GetDefaultVertexCoord(){
        static float DEFAULT_VERTEX_COORDINATE[] = {
                -1.F, -1.F,
                1.F, -1.F,
                -1.F, 1.F,
                1.F, 1.F
        };
        return DEFAULT_VERTEX_COORDINATE;
    }

    float* GetScaleVertexCoord(const int sourceWidth, const int sourceHeight, const int targetWidth,
                               const int targetHeight, RenderFrame renderFrame = FIT) {

        float target_ratio = targetWidth * 1.0F / targetHeight;
        float scale_width = 1.0F;
        float scale_height = 1.0F;
        if (renderFrame == FIT) {
            float source_ratio = sourceWidth * 1.0F / sourceHeight;
            if (source_ratio > target_ratio) {
                scale_width = 1.0F;
                scale_height = target_ratio / source_ratio;
            } else {
                scale_width = source_ratio / target_ratio;
                scale_height = 1.0F;
            }
        } else if (renderFrame == CROP) {
            float source_ratio = sourceWidth * 1.0F / sourceHeight;
            if (source_ratio > target_ratio) {
                scale_width = source_ratio / target_ratio;
                scale_height = 1.0F;
            } else {
                scale_width = 1.0F;
                scale_height = target_ratio / source_ratio;
            }
        }
        float *vertexCoordinate = new float[8];
        vertexCoordinate[0] = -scale_width;
        vertexCoordinate[1] = -scale_height;
        vertexCoordinate[2] = scale_width;
        vertexCoordinate[3] = -scale_height;
        vertexCoordinate[4] = -scale_width;
        vertexCoordinate[5] = scale_height;
        vertexCoordinate[6] = scale_width;
        vertexCoordinate[7] = scale_height;
        return vertexCoordinate;
    }

}



