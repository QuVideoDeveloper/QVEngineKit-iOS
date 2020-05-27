
#ifndef _LIB_QVAR_HPP_
#define _LIB_QVAR_HPP_

#include "amcomdef.h"
#include "qvartypedef.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // 会话追踪类型，初始化会话时配置会话追踪类型
    typedef enum _tag_qvar_tracking_type {
        QVAR_TRACKING_TYPE_NONE = 0,
        QVAR_TRACKING_TYPE_DOF6,        // 6 degree of freedom of tracking. (rotation and position -- world tracking)
        QVAR_TRACKING_TYPE_DOF3,        // 3 degree of freedom of tracking. (rotation only -- orientation tracking)
        QVAR_TRACKING_TYPE_FACE,        // unsupported yet (face tracking)
        QVAR_TRACKING_TYPE_FORCE_32BIT = 0x7FFFFFFF
    } QVARtrackingType;
    
    //会话追踪状态，回调QVARFNDidChangeState函数可以查询当前追踪状态
    typedef enum _tag_qvar_state {
        QVAR_STATE_NONE = 0,
        // Session正常工作状态或中断结束，
        QVAR_STATE_SESSION_READY,
        // AR Session由于发生某种错误而停止运行，建议结束AR效果
        QVAR_STATE_SESSION_FAILURED,
        // AR Session被临时打断(比如系统相机被临时占用，或程序临时切换至后台，或来不及处理)
        // 此时AR系统已不再处理音视频帧和跟踪相机空间位置。中断结束后状态会切换成QVAR_STATE_SESSION_READY
        // 一般可用于调整录制时间戳，比如在记录中断总时长，并用于调整中断结束后的视频帧时间，保证时间连续性。
        // 应用先进入后台，然后AR Session中断，请留意，若在该状态调用qvarfxPause，则此时不再有回调，需使用qvarfxReset/qvarfxResume重新启动
        QVAR_STATE_SESSION_INTERRUPTED,
        // 相机跟踪状态有效，此时可有效提供3D空间位置，该状态之后添加模型或物体能够展示更佳AR效果
        QVAR_STATE_TRACKING_NORMAL,
        // 相机跟踪状态依旧有效，但此时相机位置可能已经存在较大偏差，空间相对位置已不够准确，画面中物体可能存在较大抖动或漂移
        QVAR_STATE_TRACKING_LIMITED,
        // 相机跟踪状态无效，已无法有效提供相机和物体的空间3D位置，AR效果丢失，建议跟踪状态从有效变成无效后使用qvarfxReset进行重置
        QVAR_STATE_TRACKING_UNAVAILABLE,
        QVAR_STATE_FORCE_32BIT = 0x7FFFFFFF
    } QVARstate;
    
    typedef struct _tag_qvar_video_format {
        MInt32     frameRate;
        MInt32     frameWidth;
        MInt32    frameHeight;
    } QVARvideoFormat;
    
    typedef struct _tag_qvar_draw_init_desc {
        MPVoid   mtlDevice;               // instance of id<MTLDevice>, can be nil if use opengl for the rendering
        MPVoid   mtlCmdQueue;       // instance of id<MTLCommandQueue>, can be nil
        MInt32     mtlColorFormat;     // instance of MTLPixelFormat, target's color pixel format for the layer/drawable
        MInt32     mtlDepthFormat;    // instance of MTLPixelFormat, target's depth pixel format
        MInt32     mtlStencilFormat;   // instance of MTLPixelFormat, target's stencil pixel format
        MPVoid   eaglContext;           // instance of EAGLContext, can be nil if use metal for the rendering
    } QVARdrawInitDesc;
    
    typedef struct _tag_qvar_plane_property{
        MPVoid     pPlaneID;            // 平面标识(即锚点UUID)
        MFloat      fPosition[3];         // 平面在空间中的位置 (x, y, z)
        MFloat      fWidth;                 // 平面的宽度
        MFloat      fHeight;                // 平面的高度
        MInt32      mOrientation;       // 平面的垂直方向
    }QVARplaneProperty;
    
    typedef struct _tag_qvar_plane_info{
        MInt32          mCount;                 // 平面数量
        QVARplaneProperty*        planeProperties;     // 平面特性
    }QVARplaneInfo;
    
    // 请注意回调函数调用的线程可能是在其他线程，ARKit没有说明，请根据实际情况确定。 另外，请勿在这些回调函数中运行耗时的操作。
    // QVARFNDidRenderFrame不允许再调用qvarfxRender，否则将导致无限循环而崩溃
    // 回调函数在qvarfxStart之后开始启动，一般来说，其状态切换如下：
    // 1. fnDidChangeStatet通知Session状态变化，当状态为QVAR_STATE_SESSION_NORMAL开始正常运行
    // 2. fnDidUpdateAudio和fnDidUpdateFrame在状态正常后开始通知数据更新
    // 3. fnDidRenderFrame在qvarfxRender之后，请留意渲染器则在一次qvarfxRender调用后有一次fnDidRenderFrame
    // 4. fnDidUpdatePlaneInfo 其作用是ARSession中当前frame下平面信息发生变化时进行通知。
    //    平面的变化包括：检测的平面从无到有或者从有到无，平面发生合并等引起得平面信息变化。在该回调函数中会返回当前frame中的所有平面信息，包括检测到的平面数量（数量为0表示未检测到平面）和特性（标识符，长宽以及空间位置）。当没有检测到平面在current frame中的平面有变化时，返回当前frame中的所有平面信息
    typedef MRESULT (*QVARFNDidChangeState)(MHandle pUser, MHandle pCtx, MInt32 state);
    typedef MRESULT (*QVARFNDidUpdateAudio)(MHandle pUser, MHandle pCtx, MPVoid pAudio);
    typedef MRESULT (*QVARFNDidUpdateFrame)(MHandle pUser, MHandle pCtx, MPVoid pFrame, MDouble timestamp);
    typedef MRESULT (*QVARFNDidRenderFrame)(MHandle pUser, MHandle pCtx, MPVoid pTarget);
    typedef MRESULT (*QVARFNDidUpdatePlaneInfo)(MHandle pUser, MHandle pCtx, QVARplaneInfo* planeInfo);
    
    typedef struct _tag_qvar_init_desc {
        MInt32                     tracking;    // QVARtrackingType tracking type,
        MInt32                     capAudio;    // capture the audio data or not
        QVARdrawInitDesc*       drawDesc;
        QVARvideoFormat*        videoFormat;

        MPVoid                   env;
        MPVoid                   context;

        MHandle                 userData;
        QVARFNDidChangeState             fnDidChangeState;
        QVARFNDidUpdateAudio             fnDidUpdateAudio;
        QVARFNDidUpdateFrame            fnDidUpdateFrame;
        QVARFNDidRenderFrame            fnDidRenderFrame;
        QVARFNDidUpdatePlaneInfo       fnDidUpdatePlaneInfo;
    } QVARinitDesc;
    
    typedef struct _tag_qvar_draw_target_desc {
        MPVoid           context;
        MPVoid           mtlColorTexture;
        MPVoid           mtlDepthTexture;
        MPVoid           mtlSourceTex;       // Metal texture, pointer to id<MTLTexture>，非空时使用该纹理作为渲染底图，反之使用相机预览数据作为渲染底图

        MDWord    glTargetFBO;        // opengl frame buffer id
        MDWord    glSourceTex;        // OpenGL ES texture id，非零时使用该纹理作为渲染底图，反之，使用相机预览数据作为渲染底图

        MPVoid         pDrawTarget;        // a render target create by the qvar, use for render to buffer(CVPixelBuffer on iOS)
        MBool            renderScene;        // 是否渲染通过qvarfxAttachScene所添加的场景(3D模型或2D图片)，False不渲染，反之则渲染
    } QVARdrawTargetDesc;
    
    typedef enum _tag_qvar_query_key {
        QVAR_QUERY_KEY_NONE = 0,
        QVAR_QUERY_KEY_TRACKINGS,           // get all of the supported tracking type. the pData in QVARqueryVal is the pointer to an int array, each element of pData is the supported tracking type
        QVAR_QUERY_KEY_DOF6_VIDEO_FORMATS,  // get all of the supported video formats when it is world tracking. the pData in QVARqueryVal is the pointer to an QVARvideoFormat array.
        QVAR_QUERY_KEY_DOF3_VIDEO_FORMATS,  // when orientation tracking
        QVAR_QUERY_KEY_FACE_VIDEO_FORMATS,  // when face tracking
        QVAR_QUERY_KEY_FORCE_32BIT = 0x7FFFFFFF
    } QVARqueryKey;
    
    typedef struct _tag_qvar_query_value {
        MInt32     count;
        MPVoid   pData;
    } QVARqueryVal;
    
    // 查询设备支持情况, QVARqueryVal.count为支持的个数，QVARqueryVal.pData所指向的内存由外部申请和释放
    // pData的内存大小可由以下示例代码来确定
    // QVARqueryVal queryVal = { 0 };
    // qvarfxQuery(QVAR_QUERY_KEY_TRACKINGS, &queryVal);    // 查询支持的参数个数
    // if (queryVal.count > 0) 
    //      queryVal.pData = malloc(queryVal.count * sizeof(int)); 
    // qvarfxQuery(QVAR_QUERY_KEY_TRACKINGS, &queryVal);    // 查询参数数值
    // if (NULL != queryVal.pData)
    //      free(queryVal.pData);
    // 
    // 1. QVAR_QUERY_KEY_TRACKINGS, 查询AR支持的tracking类型
    //        QVARqueryVal.pData指向int数组，数组中每个元素值为QVARtrackingType
    // 2. QVAR_QUERY_KEY_***_VIDEO_FORMATS, 查询AR支持的视频帧率、宽高等参数
    //        *** 为追踪类型 （DOF6, DOF3, FACE等）
    //        QVARqueryVal.pData指向QVARvideoFormat数组
    MRESULT qvarfxQuery(QVARqueryKey queryKey, QVARqueryVal* queryVal);

    typedef struct _tag_qvar_ar_install_status_info {
        MPVoid                 env;		 		// The application's JNIEnv object
        MPVoid                 activity;			// The application_activity
        MBool                   installRequested; 	// indicates installation of ARCore
    } QVARinstallStatusInfo;

    // Android设备需要开启ARCore时调用，检查设备是否安装有合适版本的ARCore
    MRESULT qvarfxInstallStatusCheck(QVARinstallStatusInfo* pInfo);
    
    // 创建AR跟踪实例
    // 1. QVARinitDesc.tracking 跟踪类型
    //      QVAR_TRACKING_TYPE_DOF6: 6自由度跟踪，有位置远近和旋转跟踪效果
    //      QVAR_TRACKING_TYPE_DOF3: 3自由度跟踪，仅有旋转跟踪效果
    // 2. QVARinitDesc.capAudio 是否录制音频
    //      值非零时, 录制音频，音频数据在QVARinitDesc.fnDidUpdateAudio回掉函数中获取，其中pAudio为在iOS平台CMSampleBuffer实例
    //      音频数据也可以通过qvarfxGetAudioData接口获得，但需要注意的是，qvarfxGetAudioData仅保留当前的音频数据，因此可能存在部分音频数据丢失情况
    // 3. QVARinitDesc.drawDesc 渲染器初始化相关参数
    //      目前支持Metal和OpenGL ES两种不同渲染器，对应的参数可通过RenderEngine获得，或参考示例代码
    // 4. QVARinitDesc.videoFormat 预览视频帧率和宽高等参数，仅支持qvarfxQuery所返回的视频参数
    // 5. QVARinitDesc.userData 回调函数的用户句柄
    // 6. QVARinitDesc.fnDidChangeState 跟踪状态变化的回调函数
    // 7. QVARinitDesc.fnDidUpdateAudio 音频数据已更新回调接口，其中pAudio在iOS平台为CMSampleBuffer实例
    // 8. QVARinitDesc.fnDidUpdateFrame 视频预览数据已更新回调，其中pFrame在iOS平台为CVPixelBuffer实例，timestamp为当前时间
    // 9. QVARinitDesc.fnDidRenderFrame 一次渲染qvarfxRender到目标结束后的回调函数
    MRESULT qvarfxCreate(MHandle* ppCtx, QVARinitDesc* pDesc);
    
    // 销毁AR实例 ( 如内存不足等）
    MRESULT qvarfxDestroy(MHandle* ppCtx);
    
    // 更新动画时间
    MRESULT qvarfxUpdate(MHandle pCtx, MFloat fTime);
    
    // 渲染视频及模型
    // QVARdrawTargetDesc.mtlPassDesc Metal渲染目标描述
    // QVARdrawTargetDesc.mtlDrawable Metal渲染到屏幕对应的id<MTLDrawable>
    // QVARdrawTargetDesc.glTargetFBO OpenGL ES渲染目标fbo
    // QVARdrawTargetDesc.pDrawTarget 通过qvarfxMakeTarget接口创建的渲染目标
    // 渲染到屏幕时，设置QVARdrawTargetDesc 为nil
    MRESULT qvarfxRender(MHandle pCtx, QVARdrawTargetDesc* pDesc);
    
    // AR相关状态接口函数
    // qvarfxReset : 用原配置重置会话（会清除已有锚点和追踪状态）
    // qvarfxStart : 开始会话
    // qvarfxPause : 暂停会话，保留已有锚点和追踪状态（期间不再做更新），请注意调用qvarfxPause后，系统暂停所有的回调函数，请使用qvarfxReset/qvarfxResume启动
    // qvarfxResume : 恢复会话，恢复至会话暂停时状态
    MRESULT qvarfxReset(MHandle pCtx);
    MRESULT qvarfxStart(MHandle pCtx);
    MRESULT qvarfxPause(MHandle pCtx);
    MRESULT qvarfxResume(MHandle pCtx);
    
    // 设置平面检测类型
    // pOrientation 为QVAR_PLANE_DETECT_ORIENTATION_HORIZONTAL时，检测水平平面
    //                      为QVAR_PLANE_DETECT_ORIENTATION_VERTICAL时，检测垂直平面
    //                      为QVAR_PLANE_DETECT_ORIENTATION_HORIZONTALVERTICAL，检测水平和垂直平面
    //                      为QVAR_PLANE_DETECT_ORIENTATION_NONE，不检测平面
    // 默认为检测水平平面
    MRESULT qvarfxSetPlaneDetectOrientation(MHandle pCtx, MInt32 orientation);
    
    // 设置渲染viewport
    // 纹理尺寸发生变化时调用
    // pRad为ARCore调用时需设置参数，ARKit调用时设为0
    // pRect有四个元素{x,y,w,h}，(x,y)为起点位置，(w-x,h-y)分别为宽高
    MRESULT qvarfxViewport(MHandle pCtx, MInt32 rad, MFloat* pRect);
    
    // 设置渲染后图像方向
    MRESULT qvarfxSetOrientation(MHandle pCtx, MInt32 uiOrientation);
    
    // 设置渲染时相机方向
    MRESULT qvarfxSetCameraOrientation(MHandle pCtx, MInt32 camOrientation);
    
    // 创建/销毁自定义的渲染目标，用于快速获取渲染结果数据
    // NULL == pcvBuffer 时，主动创建cvBuffer
    MRESULT qvarfxMakeTarget(MHandle pCtx, MInt32 w, MInt32 h, MPVoid pcvBuffer, MPVoid* ppTarget);
    MRESULT qvarfxFreeTarget(MHandle pCtx, MPVoid* ppTarget);

    // 获取当前帧的音频数据, 其中在iOS平台ppBuffer为CMSamplerBufferRef的指针, 建议通过QVARinitDesc.fnDidUpdateAudio获取音频避免出现音频数据丢失情况
    MRESULT qvarfxGetAudioData(MHandle pCtx, MPVoid* ppBuffer);
    
    // 获取当前帧的视频数据，其中在iOS平台ppBuffer为CVPixelBufferRef指针
    //  NULL == pTarget时，ppBuffer的内容不包含所加载的3D模型，反之，ppBuffer的内容为qvarfxRender渲染之后的结果
    // 返回的时间戳 pTime 精度为毫秒级
    MRESULT qvarfxGetVideoData(MHandle pCtx, MPVoid pTarget, MPVoid* ppBuffer, MDouble* pTime);
    
    // 获取当前渲染时的viewMatrix，4x4矩阵
    MRESULT qvarfxGetViewMatrix(MHandle pCtx, MFloat* viewMatrix);
    
    // 获取当前渲染时的projectMatrix，4x4矩阵
    MRESULT qvarfxGetProjectMatrix(MHandle pCtx, MFloat* projectMatrix);
    
    // 获取pTarget对应的纹理ID
    typedef union _tag_qvar_texture_id {
        MPVoid      mtlTex;
        MDWord    oglTex;
    } QVARtexId;
    MRESULT qvarfxGetTextureid(MHandle pCtx, MPVoid pTarget, QVARtexId* pTex);
    
    typedef struct _tag_qvar_camera_project_info {
        MFloat      zFar;
        MFloat      zNear;
        MFloat      camTfm[16];
        MFloat      viewMat[16];
        MFloat      projectMat[16];
        MInt32      viewRect[4];
    } QVARcameraProjectInfo;
    
    MRESULT qvarfxGetCameraProject(MHandle pCtx, QVARcameraProjectInfo* pInfo);
    
    // hitTestResult 类型
    // PLANEPOINTHIT：优先返回平面结果，若无则返回特征点结果
    // SPACEPOINTHIT：返回特征点结果
    typedef enum _tag_hit_type {
        QVAR_HIT_TYPE_PLANEPOINTHIT = 0,
        QVAR_HIT_TYPE_SPACEPOINTHIT ,
        QVAR_HIT_TYPE_FROCE_32BIT = 0x7FFFFFFF
    } QVARhitType;

    // 工具函数，用于将屏幕2D坐标转换成3D空间位置或者4x4矩阵，其中pPoint为归一化的2D屏幕坐标点
    MRESULT qvarfxGet3DPoint(MHandle pCtx, MFloat* pPoint, MInt32 hitType, MFloat* p3DPoint);
    MRESULT qvarfxGetMatrix4(MHandle pCtx, MFloat* pPoint, MInt32 hitType, MFloat* pMatrix4);
    
    // 点击测试, pp2dIn为归一化的2D屏幕坐标点
    typedef struct _tag_qvar_hit_test_result {
        MInt32       count;
        MPVoid*    pScenes;
    } QVARhitTestResult;
    MRESULT qvarfxHitTest(MHandle pCtx, MFloat* ppt2dIn, QVARhitTestResult* pRes);
    
    // 加载3D模型, pPath为3D模型所在文件目录
    MRESULT qvarfxLoadModel(MHandle pCtx, MPCChar pPath, MPVoid* ppScene);

    // Unsupported yet
    MRESULT qvarfxLoadImage(MHandle pCtx, MPCChar pPath, MPVoid* ppScene);
    
    //加载3D文字模型，pString为输入字符串
    MRESULT qvarfxLoadText(MHandle pCtx, MPCChar pString, MPVoid* ppScene);
    
    // 释放加载的3D模型及其内存
    MRESULT qvarfxFreeScene(MHandle pCtx, MPVoid pScene);
    
    // 清楚屏幕中所有的3D模型
    MRESULT qvarfxClearAll(MHandle pCtx);

    // 将3D模型添加到屏幕中，pMat4是4x4的矩阵，可通过qvarfxGetMatrix4获得对应于屏幕坐标的矩阵
    MRESULT qvarfxAttachScene(MHandle pCtx, MPVoid pScene, MFloat* pMat4);
    
    // 克隆模型，得到克隆体的句柄
    // 若克隆 则可以添加多个相同的模型（都当前模型的复制），反之则只能添加一个相同模型
    MRESULT qvarfxCloneScene(MHandle pCtx, MPVoid pScene, MPVoid* ppScene);
    
    // 将某个3D模型从屏幕中移除
    // 如果pScene为null，则移除最新添加的模型
    MRESULT qvarfxDetachScene(MHandle pCtx, MPVoid pScene);
    
    // 将所需的音频绑定到对应模型(加载模版时无需绑定)
    MRESULT qvarfxBindAudio(MHandle pCtx, MPVoid pScene, MPVoid pAudio);

    // 设置模型的scale/position/rotation，其中旋转参数的单位为弧度
    MRESULT qvarfxSceneSetScale(MHandle pScene, MFloat scale[3]);
    MRESULT qvarfxSceneSetPosition(MHandle pScene, MFloat pos[3]);
    MRESULT qvarfxSceneSetRotation(MHandle pScene, MFloat rad[3]);
    
    // 获取模型的scale/position/rotation，其中旋转参数的单位为弧度
    MRESULT qvarfxSceneGetScale(MHandle pScene, MFloat scale[3]);
    MRESULT qvarfxSceneGetPosition(MHandle pScene, MFloat pos[3]);
    MRESULT qvarfxSceneGetRotation(MHandle pScene, MFloat rad[3]);

    // 点击拖动模型的便捷函数，主要功能为拖动屏幕中点击到的模型跟随手指位置在3D空间中移动，pPoint为归一化的2D屏幕坐标点
    // 这三个函数和UIPanGestureRecognizer的响应函数一致，分别用于响应点击开始(Touch Begin)，点击移动(Touch Move)和点击结束(Touch End)等事件
    // 可参考具体示例代码handlePanToMove:(UIGestureRecognizer*)gesturePanToMoveRecognizer
    MRESULT qvarfxMoveBegan(MHandle pCtx, MFloat* pPoint);
    MRESULT qvarfxMoving(MHandle pCtx, MFloat* pPoint);
    MRESULT qvarfxMoveEnd(MHandle pCtx);

    // 显示AR检测到的平面
    // pPath 为自定义的平面图片路径，默认使用白色显示检测到的平面，建议在qvarfxStart之前设置
    // pOrientation 为QVAR_PLANE_DISPLAY_ORIENTATION_HORIZONTAL时，显示水平平面
    //                      为QVAR_PLANE_DISPLAY_ORIENTATION_VERTICAL时，显示垂直平面
    //                      为QVAR_PLANE_DISPLAY_ORIENTATION_HORIZONTALVERTICAL，显示水平和垂直平面
    //                      为QVAR_PLANE_DISPLAY_ORIENTATION_NONE，不显示平面
    // 默认显示水平平面
    MRESULT qvarfxEnablePlaneDisplay(MHandle pCtx, MPCChar pPath, MInt32 orientation);
//    
//    MPVoid qvarfxGetARHandle();
//    MPVoid qvarfxGetSceneHandle();
#ifdef __cplusplus
}
#endif

#endif
