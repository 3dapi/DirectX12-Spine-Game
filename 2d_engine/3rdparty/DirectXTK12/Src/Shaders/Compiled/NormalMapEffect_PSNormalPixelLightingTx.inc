#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// TEXCOORD                 0   xy          0     NONE   float   xy  
// TEXCOORD                 1   xyzw        1     NONE   float   xyzw
// TEXCOORD                 2   xyz         2     NONE   float   xyz 
// COLOR                    0   xyzw        3     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][14], immediateIndexed, space=0
dcl_sampler S0[0:0], mode_default, space=0
dcl_resource_texture2d (float,float,float,float) T0[0:0], space=0
dcl_resource_texture2d (float,float,float,float) T1[1:1], space=0
dcl_resource_texture2d (float,float,float,float) T2[2:2], space=0
dcl_input_ps linear v0.xy
dcl_input_ps linear v1.xyzw
dcl_input_ps linear v2.xyz
dcl_input_ps linear v3.xyzw
dcl_output o0.xyzw
dcl_temps 9
sample r0.xyzw, v0.xyxx, T0[0].xyzw, S0[0]
mul r0.xyzw, r0.xyzw, v3.xyzw
add r1.xyz, -v1.xyzx, CB0[0][12].xyzx
dp3 r1.w, r1.xyzx, r1.xyzx
rsq r1.w, r1.w
dp3 r2.x, v2.xyzx, v2.xyzx
rsq r2.x, r2.x
mul r2.xyz, r2.xxxx, v2.xyzx
sample r3.xy, v0.xyxx, T1[1].xyzw, S0[0]
mad r3.xy, r3.xyxx, l(2.000000, 2.000000, 0.000000, 0.000000), l(-1.000000, -1.000000, 0.000000, 0.000000)
dp2 r2.w, r3.xyxx, r3.xyxx
add r2.w, -r2.w, l(1.000000)
sqrt r2.w, r2.w
deriv_rtx_coarse r4.xyz, v1.yzxy
deriv_rty_coarse r5.xyz, v1.zxyz
deriv_rtx_coarse r3.zw, v0.xxxy
deriv_rty_coarse r6.xy, v0.xyxx
mul r7.xyz, r4.xyzx, r5.yzxy
mad r7.xyz, r4.zxyz, r5.zxyz, -r7.xyzx
mul r8.xyz, r5.xyzx, r7.zxyz
mad r5.xyz, r5.zxyz, r7.xyzx, -r8.xyzx
mul r8.xyz, r4.xyzx, r7.xyzx
mad r4.xyz, r7.zxyz, r4.yzxy, -r8.xyzx
mul r6.xzw, r4.xxyz, r6.xxxx
mad r6.xzw, r3.zzzz, r5.xxyz, r6.xxzw
dp3 r3.z, r6.xzwx, r6.xzwx
rsq r3.z, r3.z
mul r6.xzw, r3.zzzz, r6.xxzw
mul r4.xyz, r4.xyzx, r6.yyyy
mad r4.xyz, r3.wwww, r5.xyzx, r4.xyzx
dp3 r3.z, r4.xyzx, r4.xyzx
rsq r3.z, r3.z
mul r4.xyz, r3.zzzz, r4.xyzx
mul r3.yzw, r3.yyyy, r4.xxyz
mad r3.xyz, r3.xxxx, r6.xzwx, r3.yzwy
mad r2.xyz, r2.wwww, r2.xyzx, r3.xyzx
dp3 r2.w, r2.xyzx, r2.xyzx
rsq r2.w, r2.w
mul r2.xyz, r2.wwww, r2.xyzx
mad r3.xyz, r1.xyzx, r1.wwww, -CB0[0][3].xyzx
dp3 r2.w, r3.xyzx, r3.xyzx
rsq r2.w, r2.w
mul r3.xyz, r2.wwww, r3.xyzx
mad r4.xyz, r1.xyzx, r1.wwww, -CB0[0][4].xyzx
dp3 r2.w, r4.xyzx, r4.xyzx
rsq r2.w, r2.w
mul r4.xyz, r2.wwww, r4.xyzx
mad r1.xyz, r1.xyzx, r1.wwww, -CB0[0][5].xyzx
dp3 r1.w, r1.xyzx, r1.xyzx
rsq r1.w, r1.w
mul r1.xyz, r1.wwww, r1.xyzx
dp3 r5.x, -CB0[0][3].xyzx, r2.xyzx
dp3 r5.y, -CB0[0][4].xyzx, r2.xyzx
dp3 r5.z, -CB0[0][5].xyzx, r2.xyzx
dp3 r3.x, r3.xyzx, r2.xyzx
dp3 r3.y, r4.xyzx, r2.xyzx
dp3 r3.z, r1.xyzx, r2.xyzx
ge r1.xyz, r5.xyzx, l(0.000000, 0.000000, 0.000000, 0.000000)
and r1.xyz, r1.xyzx, l(0x3f800000, 0x3f800000, 0x3f800000, 0)
mul r2.xyz, r5.xyzx, r1.xyzx
max r3.xyz, r3.xyzx, l(0.000000, 0.000000, 0.000000, 0.000000)
mul r1.xyz, r1.xyzx, r3.xyzx
log r1.xyz, r1.xyzx
mul r1.xyz, r1.xyzx, CB0[0][2].wwww
exp r1.xyz, r1.xyzx
mul r1.xyz, r5.xyzx, r1.xyzx
mul r3.xyz, r2.yyyy, CB0[0][7].xyzx
mad r2.xyw, r2.xxxx, CB0[0][6].xyxz, r3.xyxz
mad r2.xyz, r2.zzzz, CB0[0][8].xyzx, r2.xywx
mad r2.xyz, r2.xyzx, CB0[0][0].xyzx, CB0[0][1].xyzx
mul r3.xyz, r1.yyyy, CB0[0][10].xyzx
mad r1.xyw, r1.xxxx, CB0[0][9].xyxz, r3.xyxz
mad r1.xyz, r1.zzzz, CB0[0][11].xyzx, r1.xywx
mul r1.xyz, r1.xyzx, CB0[0][2].xyzx
sample r3.xyz, v0.xyxx, T2[2].xyzw, S0[0]
mul r1.xyz, r1.xyzx, r3.xyzx
mul r1.xyz, r0.wwww, r1.xyzx
mad r0.xyz, r0.xyzx, r2.xyzx, r1.xyzx
mad r1.xyz, CB0[0][13].xyzx, r0.wwww, -r0.xyzx
mad o0.xyz, v1.wwww, r1.xyzx, r0.xyzx
mov o0.w, r0.w
ret 
// Approximately 0 instruction slots used
#endif

const BYTE NormalMapEffect_PSNormalPixelLightingTx[] =
{
     68,  88,  66,  67, 206, 227, 
    133, 133, 205, 163, 146,  99, 
     68, 182, 144, 140, 174, 149, 
      9, 232,   1,   0,   0,   0, 
    232,  12,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    176,   0,   0,   0, 228,   0, 
      0,   0, 232,  11,   0,   0, 
     73,  83,  71,  78, 120,   0, 
      0,   0,   4,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   3,   0,   0, 104,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  15,   0,   0, 104,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   7,   0,   0, 113,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,  15,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,   0, 171, 171, 
     83,  72,  69,  88, 252,  10, 
      0,   0,  81,   0,   0,   0, 
    191,   2,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   7, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  14,   0, 
      0,   0,   0,   0,   0,   0, 
     90,   0,   0,   6,  70, 110, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     88,  24,   0,   7,  70, 126, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
      0,   0,   0,   0,  88,  24, 
      0,   7,  70, 126,  48,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,   0,   0, 
      0,   0,  88,  24,   0,   7, 
     70, 126,  48,   0,   2,   0, 
      0,   0,   2,   0,   0,   0, 
      2,   0,   0,   0,  85,  85, 
      0,   0,   0,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   0,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 114,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   3,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   9,   0, 
      0,   0,  69,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  96, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  30,  16,   0,   3,   0, 
      0,   0,   0,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,  18,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  12,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     16,   0,   0,   7,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  68,   0, 
      0,   5,  18,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   2,   0,   0,   0, 
      6,   0,  16,   0,   2,   0, 
      0,   0,  70,  18,  16,   0, 
      2,   0,   0,   0,  69,   0, 
      0,  11,  50,   0,  16,   0, 
      3,   0,   0,   0,  70,  16, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,  96,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,  15,  50,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   0,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  64,   0,   0, 
      0,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128, 191, 
      0,   0, 128, 191,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   7, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   3,   0, 
      0,   0,  70,   0,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   8, 130,   0,  16,   0, 
      2,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     75,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0, 122,   0,   0,   5, 
    114,   0,  16,   0,   4,   0, 
      0,   0, 150,  20,  16,   0, 
      1,   0,   0,   0, 124,   0, 
      0,   5, 114,   0,  16,   0, 
      5,   0,   0,   0,  38,  25, 
     16,   0,   1,   0,   0,   0, 
    122,   0,   0,   5, 194,   0, 
     16,   0,   3,   0,   0,   0, 
      6,  20,  16,   0,   0,   0, 
      0,   0, 124,   0,   0,   5, 
     50,   0,  16,   0,   6,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      7,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
    150,   4,  16,   0,   5,   0, 
      0,   0,  50,   0,   0,  10, 
    114,   0,  16,   0,   7,   0, 
      0,   0,  38,   9,  16,   0, 
      4,   0,   0,   0,  38,   9, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   7,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   8,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  38,   9,  16,   0, 
      7,   0,   0,   0,  50,   0, 
      0,  10, 114,   0,  16,   0, 
      5,   0,   0,   0,  38,   9, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   7,   0, 
      0,   0,  70,   2,  16, 128, 
     65,   0,   0,   0,   8,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   8,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   7,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   4,   0,   0,   0, 
     38,   9,  16,   0,   7,   0, 
      0,   0, 150,   4,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   7, 210,   0,  16,   0, 
      6,   0,   0,   0,   6,   9, 
     16,   0,   4,   0,   0,   0, 
      6,   0,  16,   0,   6,   0, 
      0,   0,  50,   0,   0,   9, 
    210,   0,  16,   0,   6,   0, 
      0,   0, 166,  10,  16,   0, 
      3,   0,   0,   0,   6,   9, 
     16,   0,   5,   0,   0,   0, 
      6,  14,  16,   0,   6,   0, 
      0,   0,  16,   0,   0,   7, 
     66,   0,  16,   0,   3,   0, 
      0,   0, 134,   3,  16,   0, 
      6,   0,   0,   0, 134,   3, 
     16,   0,   6,   0,   0,   0, 
     68,   0,   0,   5,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
    210,   0,  16,   0,   6,   0, 
      0,   0, 166,  10,  16,   0, 
      3,   0,   0,   0,   6,  14, 
     16,   0,   6,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  86,   5,  16,   0, 
      6,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      4,   0,   0,   0, 246,  15, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  16,   0, 
      0,   7,  66,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  68,   0,   0,   5, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      4,   0,   0,   0, 166,  10, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
    226,   0,  16,   0,   3,   0, 
      0,   0,  86,   5,  16,   0, 
      3,   0,   0,   0,   6,   9, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   3,   0,   0,   0, 
      6,   0,  16,   0,   3,   0, 
      0,   0, 134,   3,  16,   0, 
      6,   0,   0,   0, 150,   7, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      2,   0,   0,   0,  58,   0, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  12, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70, 130,  48, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  16,   0, 
      0,   7, 130,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  68,   0,   0,   5, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      3,   0,   0,   0, 246,  15, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  12, 
    114,   0,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  48, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  16,   0,   0,   7, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   4,   0, 
      0,   0, 246,  15,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  12, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70, 130, 
     48, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  16,   0, 
      0,  10,  18,   0,  16,   0, 
      5,   0,   0,   0,  70, 130, 
     48, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  16,   0,   0,  10, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  70, 130,  48, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     16,   0,   0,  10,  66,   0, 
     16,   0,   5,   0,   0,   0, 
     70, 130,  48, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  16,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  16,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     16,   0,   0,   7,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  29,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  52,   0, 
      0,  10, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     47,   0,   0,   5, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   9, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0, 246, 143, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  25,   0,   0,   5, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   9, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  86,   5,  16,   0, 
      2,   0,   0,   0,  70, 130, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  50,   0,   0,  11, 
    178,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      2,   0,   0,   0,  70, 136, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  70,   8,  16,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      2,   0,   0,   0, 166,  10, 
     16,   0,   2,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  70,   3, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,  13, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   9, 114,   0,  16,   0, 
      3,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  50,   0, 
      0,  11, 178,   0,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 136,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  70,   8, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  11, 114,   0, 
     16,   0,   1,   0,   0,   0, 
    166,  10,  16,   0,   1,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  11,   0,   0,   0, 
     70,   3,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   9, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70, 130, 
     48,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  69,   0,   0,  11, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     32,   0,   2,   0,   0,   0, 
      2,   0,   0,   0,   0,  96, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  12, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     13,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,  32, 
     16,   0,   0,   0,   0,   0, 
    246,  31,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 130,  32, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     82,  84,  83,  48, 248,   0, 
      0,   0,   2,   0,   0,   0, 
      6,   0,   0,   0,  24,   0, 
      0,   0,   0,   0,   0,   0, 
    248,   0,   0,   0,  29,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  96,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0, 192,   0, 
      0,   0,   2,   0,   0,   0, 
      1,   0,   0,   0, 204,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0, 216,   0, 
      0,   0,   1,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   1,   0, 
      0,   0, 136,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0, 168,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
    224,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255
};
