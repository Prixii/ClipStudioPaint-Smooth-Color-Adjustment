#ifndef CSPMODGeometry_h
#define CSPMODGeometry_h

#include<SDL3/SDL.h>
#include<vector>


namespace geo
{
    struct PointF
    {
        float x = 0;
        float y = 0;
    };



    inline float cross(const PointF& a, const PointF& b) {
        return a.x * b.y - a.y * b.x;
    }

    inline bool isPointInTriangle(const PointF& p, const PointF& t1,const PointF& t2, const PointF& t3) {
        return cross({ p.x - t1.x ,p.y-t1.y}, { t2.x - t1.x,t2.y - t1.y }) > 0.f &&
            cross({ p.x - t2.x ,p.y - t2.y }, { t3.x - t2.x,t3.y - t2.y }) > 0.f &&
            cross({ p.x - t3.x ,p.y - t3.y }, { t1.x - t3.x,t1.y - t3.y }) > 0.f;
    }


    inline float polygonArea(const std::vector<PointF>& poly) {
        if (poly.size() < 3) return 0.0;

        float area = 0.0;
        int n = (int)poly.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            area += cross(poly[i], poly[j]);
        }
        return SDL_fabsf(area) * 0.5f;
    }

    inline PointF lineIntersection(const PointF& p1, const PointF& p2, const PointF& p3, const PointF& p4) {
        float dx1 = p2.x - p1.x, dy1 = p2.y - p1.y;
        float dx2 = p4.x - p3.x, dy2 = p4.y - p3.y;

        // 避免除以0，实际使用中需处理平行情况
        float denom = dx1 * dy2 - dy1 * dx2;
        if (SDL_fabsf(denom) < 1e-6) return PointF();

        float dx3 = p3.x - p1.x, dy3 = p3.y - p1.y;
        float t = (dx2 * dy3 - dy2 * dx3) / denom;

        return PointF{ p1.x + t * dx1, p1.y + t * dy1 };
    }


    inline std::vector<PointF> clipPolygon(const std::vector<PointF>& inputPoly,
        const PointF& clipStart, const PointF& clipEnd,
        bool (*insideTest)(const PointF&, const PointF&, const PointF&)) {
        std::vector<PointF> outputList = inputPoly;
        std::vector<PointF> inputList;

        if (outputList.empty()) return outputList;

        // 依次用四条边裁剪：左、右、下、上 (顺序不重要，只要围成一圈)
        // 这里我们需要针对像素框的四条边分别调用裁剪
        // 为了简化，这里只写单次裁剪的逻辑，下面会组合使用

        inputList = outputList;
        outputList.clear();

        if (inputList.empty()) return outputList;

        PointF S = inputList.back();
        for (const PointF& E : inputList) {
            bool insideE = insideTest(E, clipStart, clipEnd);
            bool insideS = insideTest(S, clipStart, clipEnd);

            if (insideE) {
                if (!insideS) {
                    // 从外进入内：添加交点
                    outputList.push_back(lineIntersection(S, E, clipStart, clipEnd));
                }
                // 添加终点
                outputList.push_back(E);
            }
            else if (insideS) {
                // 从内穿出到外：添加交点
                outputList.push_back(lineIntersection(S, E, clipStart, clipEnd));
            }
            S = E;
        }
        return outputList;
    }

    // 判断点 P 是否在直线 AB 的左侧 (或右侧，取决于 winding order)
    // 这里我们定义“内侧”为：对于顺时针排列的矩形框，内侧在边的右侧
    bool isInsideRight(const PointF& p, const PointF& a, const PointF& b) {
        // 叉积 < 0 表示在右侧 (顺时针方向)
        return cross(PointF{ b.x - a.x, b.y - a.y }, PointF{ p.x - a.x, p.y - a.y }) <= 0;
    }



    /**
     * 计算三角形与像素方框的交集面积
     * @param t1, t2, t3 三角形顶点
     * @param px, py 像素左上角坐标
     * @return 交集面积
     */
    float calculateIntersectionArea(PointF t1, PointF t2, PointF t3, int px, int py) {
        // 1. 初始化多边形为三角形
        std::vector<PointF> poly = { t1, t2, t3 };

        // 2. 定义像素方框的四个角 (顺时针)
        // 左上(TL), 右上(TR), 右下(BR), 左下(BL)
        PointF TL{ (float)px, (float)py };
        PointF TR{ (float)px + 1,(float)py };
        PointF BR{ (float)px + 1, (float)py + 1 };
        PointF BL{ (float)px,(float)py + 1 };

        // 3. 依次用像素框的四条边裁剪三角形
        // 注意：insideTest 必须与矩形顶点的顺序一致 (顺时针用 isInsideRight)

        // 裁剪上边 (TL -> TR)
        poly = clipPolygon(poly, TL, TR, isInsideRight);
        if (poly.empty()) return 0.0;

        // 裁剪右边 (TR -> BR)
        poly = clipPolygon(poly, TR, BR, isInsideRight);
        if (poly.empty()) return 0.0;

        // 裁剪下边 (BR -> BL)
        poly = clipPolygon(poly, BR, BL, isInsideRight);
        if (poly.empty()) return 0.0;

        // 裁剪左边 (BL -> TL)
        poly = clipPolygon(poly, BL, TL, isInsideRight);
        if (poly.empty()) return 0.0;

        // 4. 计算剩下的多边形面积
        return polygonArea(poly);
    }

    float getPixelAlpha(PointF t1, PointF t2, PointF t3, int px, int py) {
        float area = calculateIntersectionArea(t1, t2, t3, px, py);
        // 像素总面积是 1x1 = 1，所以面积直接等于不透明度
        return std::min(1.0f, std::max(0.0f, area));
    }
}


#endif
