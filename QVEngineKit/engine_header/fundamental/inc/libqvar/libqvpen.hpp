
#ifndef _LIB_QV_PEN_HPP_
#define _LIB_QV_PEN_HPP_
#include "amdisplay.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef enum _tag_qv_pen_type {
        QV_PEN_TYPE_NONE = 0,
        QV_PEN_TYPE_2D,         // 2D涂鸦
        QV_PEN_TYPE_3D_RIBBON,         // 3D画线，通过传入qvarfx实例可以实现AR空间画线功能
        QV_PEN_TYPE_3D_CYLINDER,
        QV_PEN_TYPE_FORCE_32BIT = 0x7FFFFFFF
    } QVpenType;
    
    typedef struct _tag_qv_pen_init_desc {
        MLong     penType;            // 画笔类型
        // 当传入mtlDevice有效时，qvpen使用Metal进行渲染，当eaglContext有效时，则使用OpenGL ES进行渲染
        // 当qvpen和其他渲染模块共存时，请注意传入公用的参数，比如使用Metal渲染时，需传入公用的id<MTLDevice>和id<MTLCommandQueue>
        MPVoid    mtlDevice;          // instance of id<MTLDevice>, can be nil if use opengl for the rendering
        MPVoid    mtlCmdQueue;        // instance of id<MTLCommandQueue>, can be nil
        MPVoid    eaglContext;        // instance of EAGLContext, can be nil if use metal for the rendering
    } QVpenInitDesc;

    typedef struct _tag_qv_pen_draw_desc {
        // 以下三个以mtl打头的参数用于Metal渲染
        MPVoid           context;
        // Metal渲染时的渲染目标，
        MPVoid           mtlColorTexture;
        MPVoid           mtlDepthTexture;
        // 将Metal渲染结果显示至屏幕的参数，如果渲染到纹理，请将mtlDrawable设置为NULL
        //MPVoid           mtlDrawable;        // pointer to id<MTLDrawable>
        // 使用OpenGL ES渲染时的渲染目标
        MDWord           glTargetFBO;        // opengl frame buffer id
        // 是否清屏
        MBool            clearTarget;
    } QVpenDrawDesc;
    
    typedef union _tag_qv_pen_tex {
        // OpenGL纹理ID
        MDWord            oglTex;
        // Metal纹理句柄
        MPVoid            mtlTex;
    } QVpenTex;
    
    // qvpen使用Metal/OpenGL ES进行渲染，所有接口函数都不是线程安全的，并且由于GPU限制，所有接口都不能在后台执行。
    // qvpen按照状态机的准则来处理画笔有关的参数，即线段生成前所设置的参数才作为该线段的绘制参数。请在绘制线段前设置画笔有关参数。
    // 比如一下代码：
    //      qvpenSetWidth(pPen, 4.0f);
    //      qvpenTouchBegin(pPen, ...); qvpenTouchMoved(pPen, ...); qvpenTouchEnded(pPen);  // 线段一
    //      float rgba[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //      qvpenSetColor（pPen, rgba);
    //      qvpenTouchBegin(pPen, ...); qvpenTouchMoved(pPen, ...); qvpenTouchEnded(pPen);  // 线段二
    // 则使用qvpenRender出来的结果是线段一为宽度为4的白色(默认值)线段，线段二为宽度为4的红色线段

    MRESULT qvpenCreate(MHandle* ppPen, QVpenInitDesc* pDesc);
    
    MRESULT qvpenDestroy(MHandle pPen);
    
	// 将画笔中的渲染到指定的目标(屏幕或者纹理)
    MRESULT qvpenRender(MHandle pPen, QVpenDrawDesc* pDesc);
    
    // viewport参数为float[4]数组，依次存放left, top, right, bottom,以像素为单位
    MRESULT qvpenSetViewRect(MHandle pPen, MFloat* viewport);

    // 设置画笔线宽，建议值范围为[4.0f, 20.0f]，具体物理函数待效果完善后再详细说明
    MRESULT qvpenSetWidth(MHandle pPen, MFloat width);
    
    // 3Dbrush画笔专用，设置画笔粗糙度
    MRESULT qvpenSetRoughness(MHandle pPen, MFloat roughness);
    
    // 3Dbrush画笔专用，设置画笔金属度
    MRESULT qvpenSetMetalness(MHandle pPen, MFloat metalness);
    
    // 设置画笔颜色，pRGBA参数为float[4]数组，每个颜色分量的范围为[0.0f ~ 1.0f]，默认画笔颜色为白色
    MRESULT qvpenSetColor(MHandle pPen, MFloat* StartPRGBA, MFloat* EndedPRGBA);
    
    // 设置循环颜色环
    MRESULT qvpenSetCircularColor(MHandle pPen, MBool bCirColor);
   
    // 将画笔作为橡皮，即擦除画笔所经过的路线上的颜色，3D画笔不支持该功能(接口可调用，但无效果)
    MRESULT qvpenAsEraser(MHandle pPen, MBool asEraser);
    
    // 设置画笔的纹理，将外部生成的纹理作为画笔的纹理，请保证该纹理在画笔绘制期间有效，反之将出现不可预料异常。
    MRESULT qvpenSetTexName(MHandle pPen, QVpenTex* pTex);
    
    // 设置画笔纹理，将解码后的图片数据作为画笔纹理，该接口仅支持格式为BGRA8图片数据，由内部转为RGBA8颜色空间，每行的字节数为width * 4
    MRESULT qvpenSetTexData(MHandle pPen, MBITMAP bitmap);
    
    // 设置画笔法线纹理，将解码后的图片数据作为画笔纹理，该接口仅支持格式为BGRA8图片数据，由内部转为RGBA8颜色空间，每行的字节数为width * 4
    MRESULT qvpenSetNormalTexData(MHandle pPen, MBITMAP bitmap);
    
    // 设置画笔粗糙度纹理，将解码后的图片数据作为画笔纹理，该接口仅支持格式为BGRA8图片数据，由内部转为RGBA8颜色空间，每行的字节数为width * 4
    MRESULT qvpenSetRoughnessTexData(MHandle pPen, MBITMAP bitmap);
    
    // 设置画笔金属度纹理，将解码后的图片数据作为画笔纹理，该接口仅支持格式为BGRA8图片数据，由内部转为RGBA8颜色空间，每行的字节数为width * 4
    MRESULT qvpenSetMetalnessTexData(MHandle pPen, MBITMAP bitmap);
    
    // 设置画笔高度纹理，将解码后的图片数据作为画笔纹理，该接口仅支持格式为BGRA8图片数据，由内部转为RGBA8颜色空间，每行的字节数为width * 4
    MRESULT qvpenSetHeightTexData(MHandle pPen, MBITMAP bitmap);
    
    // 设置qvarfx实例，使3D画笔功能具备AR空间画线功能，请确保qvarfx实例的生命周期长于qvpen
    MRESULT qvpenSetQVARFX(MHandle pPen, MPVoid pARFX);
    
    //返回画笔类型2D/3D画笔，返回QVpenType值
    MLong qvpenGetPenType(MHandle pPen);
    
    // 画笔绘制线段函数，其中p2dPoint为归一化的2D屏幕坐标点
    MRESULT qvpenTouchBegin(MHandle pPen, MFloat* p2dPoint);   // 线段起始点
    MRESULT qvpenTouchMoved(MHandle pPen, MFloat* p2dPoint);   // 线段经过的某一点, 将该点添加到当前的线段中
    MRESULT qvpenTouchEnded(MHandle pPen);                    // 结束一个线段
    
    
    // 以下接口提供画笔线段生成/获取线段数据的操作功能，可用于保存/加载线段数据。通过设置线段数据，可以完成不同的线段动画
    
    // 返回当前画笔中线段数量
    MLong qvpenGetLineNum(MHandle pPen);
    
    // 清空当前画笔中所有线段
    MRESULT qvpenClearAll(MHandle pPen);
    
    // 将归一化的2D屏幕坐标点映射成3D空间坐标(与当前相机位置信息有关)，若是2D画笔，返回的3D空间点的Z坐标值为0
    // d2Point: 输入参数, 归一化的2D屏幕坐标, float[2];
    // d3Point: 输出参数, 3D空间坐标点, float[3];
    MRESULT qvpenUnprojectPoint(MHandle pPen, MFloat* d2Point, MFloat* d3Point);

    
    // 以下是新接口
    // 画笔内部线段集合末尾创建一条线段,并设置线段的点数据,所生成的线段索引是qvpenGetLineNum() - 1,返回句柄
    // pointArray: 是指向线段点集的float数组，是归一化的2D/3D坐标点，如果是2D画笔，每个点为2个float，如果是3D画笔，每个点为3个float（可以为NULL）
    // pointCount: 线段点的个数，float数组元素的数量为count * 2(或3)
    // ppLine:返回该线段的句柄ppLine
    MRESULT qvpenCreateLine(MHandle pPen, MFloat* pointArray, MLong pointCount, MHandle* ppLine);
    
    // 根据句柄销毁线段，销毁后句柄pLine请设置为空，以防出错。
    // pLine:句柄
    MRESULT qvpenDestroyLine(MHandle pLine);
    
    
    // 根据索引获取该线段的句柄
    // index:索引值
    // ppLine:返回该线段的句柄ppLine
    MRESULT qvpenGetLineHandle(MHandle pPen, MLong lineIndex, MHandle* ppLine);
    
    // 根据句柄获取该线段的索引,返回值为该线段的索引
    // pLine:句柄
    MLong qvpenGetLineIndex(MHandle pLine);
    
    // 根据句柄设置该线段的宽度信息
    // pLine:句柄
    // width:线宽
    MRESULT qvpenSetLineWidth(MHandle pLine, MFloat width);
    
    // 根据句柄设置该线段的颜色信息，如果线条颜色为渐变色StartPRGBA和EndedPRGBA参数为float[4]数组，每个颜色分量的范围为[0.0f ~ 1.0f]
    // 如果画笔为单一色, 则设置StartPRGBA等于EndedPRGBA或设置两者中一个为NULL
    // pLine:句柄
    // StartPRGBA:起始颜色
    // EndedPRGBA:终点颜色
    MRESULT qvpenSetLineColor(MHandle pLine, MFloat* StartPRGBA, MFloat* EndedPRGBA);
    // 根据句柄获取该线段的数据信息
    // pLine:句柄
    // pointArray:归一化后的2D/3D坐标点，如果是2D画笔，每个点为2个float，如果是3D画笔，每个点为3个float
    // ptCount:线段点的个数
    // pLineLen:该线段的屏幕像素长度
    // pointArray内存由外部申请，可以通过两次调用来获取线段的数据
    // 第一次调用获得线段的个数
    // MLong ptCount = 0;
    // float lineLen = 0.0f;
    // float* ptArray = NULL;
    // qvpenLineGetData(void* pLine, NULL, &ptCount, NULL);
    // 第二次调用获取线段的点数据
    // if (ptCount > 0) {
    //      int ptNum = 2;  // 如果是2D画笔，每个点为2个float，如果是3D画笔，每个点为3个float
    //      ptArray = (float*)malloc(sizeof(float) * 2 * ptCount);
    //      if (NULL != ptArray)
    //          qvpenLineGetData(void* pLine, float* pointArray, &ptCount, &lineLen);
    // }
    // 两个点之间屏幕像素距离的计算,比如一下代码：
    //   float pLhs[2];//存储当前屏幕像素坐标A1(x,y)
    //   float pRhs[2];//存储当前屏幕像素坐标A2(x,y)
    //   float dx = pLhs[0] - pRhs[0];
    //   float dy = pLhs[1] - pRhs[1];
    //   float length=sqrtf(dx * dx + dy * dy);
    MRESULT qvpenGetLineData(MHandle pLine, MFloat* pointArray, MLong* ptCount,MFloat* pLineLen);
    
    // 根据句柄设置该线段的数据信息
    // pLine:句柄
    // pointArray:归一化后的2D/3D坐标点，如果是2D画笔，每个点为2个float，如果是3D画笔，每个点为3个float
    // ptCount:线段点的个数
    MRESULT qvpenSetLineData(MHandle pLine, MFloat* pointArray, MLong ptCount);
    
    // 根据句柄设置该线段的渲染顺序号，顺序号笑的先渲染。
    // pLine:句柄
    // drawOrder:渲染序号
    MRESULT qvpenSetLineDrawOrder(MHandle pLine, MLong drawOrder);
    
    // 根据句柄获取该线段的渲染顺序号
    // pLine:句柄
    // *drawOrder:返回渲染序号
    MRESULT qvpenGetLineDrawOrder(MHandle pLine, MLong* drawOrder);
    
	
    typedef struct _tag_qv_pen_tex_data {
        MLong width;
        MLong height;
        MVoid* pData;
    }QVpenMosaicTexData;
     
    typedef struct _tag_qv_pen_mask_data {
        MLong count;
        MLong quota;
        QVpenMosaicTexData* maskTexData;
    }QVpenMaskTexList;
    
    typedef enum _tag_pen_effect_type {
        QV_PEN_EFFECT_TYPE_NONE = 0,
        QV_PEN_EFFECT_TYPE_MOSAIC_RECT,
        QV_PEN_EFFECT_TYPE_MOSAIC_CIRCLE,
        QV_PEN_EFFECT_TYPE_MOSAIC_TRANGLE,
        QV_PEN_EFFECT_TYPE_MOSAIC_BOTTOMMAP,
        QV_PEN_EFFECT_TYPE_MOSAIC_SUB_BOTTOMMAP,
        QV_PEN_EFFECT_TYPE_MOSAIC_TEMPLET,
        QV_PEN_EFFECT_TYPE_FORCE_32BIT = 0x7FFFFFFF
    } QVpenEffectType;
     
    /*暂时没有用到的接口
    // 2D画笔设置背景纹理
    // 3D画笔空实现，无作用
    MRESULT qvpenSetBackgroundTexName(MPVoid pPen, QVpenTex* pTex);
     
    // 设置背景纹理，将解码后的图片数据作为背景纹理，该接口仅支持格式为RGBA8的图片数据，每行的字节数为width * 4
    // 3D画笔空实现，无作用
    MRESULT qvpenSetBackgroundTexData(MPVoid pPen, MLong width, MLong height, MPVoid pData);
    
    // 画笔应用不同的马赛克效果,不同的画笔类型对应不同的马赛克效果,各参数值得意义如下所示
    //     ------------—---                       ----------------—-----
    //     类型（qvpen2DType）                      马赛克纹理 (pData）
    //     ------------—---                       ----------------—-----
    // QV_PEN_EFFECT_TYPE_NONE (默认纯色)                  NULL
    // QV_PEN_EFFECT_TYPE_MOSAIC_RECT (矩形马赛克)          NULL
    // QV_PEN_EFFECT_TYPE_MOSAIC_CIRCLE (圆形马赛克)        NULL
    // QV_PEN_EFFECT_TYPE_MOSAIC_TRANGLE(三角形马赛克)      NULL
    // QV_PEN_EFFECT_TYPE_MOSAIC_BOTTOMMAP(刮大图)       mosaicTexData(大)
    // QV_PEN_EFFECT_TYPE_MOSAIC_SUB_BOTTOMMAP(刮拓展小图)mosaicTexData(小)
    // QV_PEN_EFFECT_TYPE_MOSAIC_TEMPLET(拼图)           maskTexList(一张素材或多张素材模板)
    
    MRESULT qvpenSetEffect(MPVoid pPen, MLong effectType, MPVoid pData);
     
    /*弃用老接口
    MRESULT qvpenAddLine(MPVoid pPen, MFloat* pointArray, MLong pointCount);
    MRESULT qvpenSetLineColor(MPVoid pPen, MLong lineIndex, MFloat* StartPRGBA ,MFloat* EndedPRGBA);
    MRESULT qvpenSetLineWidth(MPVoid pPen, MLong lineIndex, MFloat width);
    MRESULT qvpenGetLineData(MPVoid pPen, MLong lineIndex, MFloat* linePoint, MLong* count, MFloat* pLineLen);
    MRESULT qvpenSetLineData(MPVoid pPen, MLong lineIndex, MFloat* pointArray, MLong pointCount);*/
    
    
#ifdef __cplusplus
}
#endif

#endif
