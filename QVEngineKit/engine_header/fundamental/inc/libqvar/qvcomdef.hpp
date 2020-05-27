
#ifndef _QV_COMMON_DEFINE_HPP_
#define _QV_COMMON_DEFINE_HPP_

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct _tag_qv_pen_ogl_tex_name{
        unsigned int    oglPenTex;        // OpenGL ES texture name
        unsigned int    oglBackgroundTex; // OpenGL ES texture name
        unsigned int    oglMasaiTex;      // OpenGL ES texture name
    } QVpenOglTexName;
    
    typedef struct _tag_qv_pen_mtl_tex_name{
        void*    mtlPenTex;        // MTL texture name
        void*    mtlBackgroundTex; // MTL texture name
        void*    mtlMasaiTex;      // MTL texture name
    } QVpenMtlTexName;
    
    typedef union _tag_qv_pen_tex_name {
        QVpenMtlTexName  mtlTex;     // instance of id<MTLTexture>
        QVpenOglTexName  oglTex;     // OpenGL ES texture name
    } QVpenTexName;
    
    typedef union _tag_qv_ptr_id {
        void*           ptr;
        size_t          val;
        unsigned int    uid;
    } QVptrid;

    typedef union _tag_qv_float_2 {
        struct {
            float x;
            float y;
        };
        struct {
            float u;
            float v;
        };
        float f[2];
    } QVfloat2;

    typedef union _tag_qv_float_3 {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float r;
            float g;
            float b;
        };
        float f[3];
    } QVfloat3;

    typedef union _tag_qv_float_4 {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        struct {
            float r;
            float g;
            float b;
            float a;
        };
        float f[4];
    } QVfloat4;
    
    typedef union _tag_qv_float_5 {
        struct {
            float x;
            float y;
            float z;
            float u;
            float v;
        };
        struct {
            float r;
            float g;
            float b;
            float m;
            float n;
        };
        float f[5];
    } QVfloat5;

    typedef QVfloat2  QV2Dpoint;
    typedef QVfloat4  QVcolor;
    typedef QVfloat3  QV3Dpoint;
    typedef QVfloat5  QV3DpointTexCoord;
    

    typedef struct _tag_qv_rect {
        int l;
        int t;
        int r;
        int b;
    } QVrect;

    typedef union _tag_qv_matrix_4x4 {
        float m[16];
        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        QVfloat4 v[4];
    } QVmat4x4;

    typedef union _tag_qv_matrix_3x3 {
        float m[9];
        struct {
            float m00, m01, m02;
            float m10, m11, m12;
            float m20, m21, m22;
        };
        QVfloat3 v[3];
    } QVmat3x3;

    typedef union _tag_qv_matrix_2x2 {
        float m[4];
        struct {
            float m00, m01;
            float m10, m11;
        };
        QVfloat2 v[2];
    } QVmat2x2;

#ifdef __cplusplus
}
#endif
#endif
