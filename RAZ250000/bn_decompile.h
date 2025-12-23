/* Header for BinaryNinja Decompile helpers
 * Declares forward prototypes and externs used by other reverse-compiled units.
 */
#ifndef __BN_DECOMPILE_H__
#define __BN_DECOMPILE_H__

#include <stdint.h>
#include <stdbool.h>
#include "n32g031.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Full set of prototypes exported from BinaryNinja Decompile.c
 * Types are conservative (int32_t / uint32_t / void* / char / int16_t*).
 * If you have more precise types, replace the signatures accordingly.
 */

int32_t sub_0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6);
int32_t sub_c8(void);
int32_t sub_d8(void);
int32_t sub_f4(int32_t arg1, int32_t arg2);
int32_t sub_120(int32_t arg1, int32_t arg2);
void sub_148(int32_t arg1, int32_t arg2);
int32_t sub_1ea(int32_t arg1, int32_t arg2);
int32_t sub_1f2(int32_t arg1, int32_t arg2);
int32_t sub_1fa(int32_t arg1, int32_t arg2);
int32_t sub_274(int32_t arg1, int32_t arg2);
int32_t sub_2f0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_3ba(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_418(uint32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_41e(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_43a(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_446(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_458(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_528(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_618(uint32_t arg1);
int32_t sub_634(int32_t arg1);
int32_t sub_65c(int32_t arg1, int32_t arg2);
uint32_t sub_698(int32_t arg1);
void sub_6c0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
void sub_6e8(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_710(int32_t arg1, int32_t arg2);
void sub_748(int32_t arg1, int32_t arg2);
int32_t sub_75c(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_77c(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_79e(int32_t arg1, int32_t arg2, int32_t arg3);
void sub_7c4(uint32_t arg1, int32_t arg2);
int32_t sub_7d4(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
void sub_846(uint32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_860(uint32_t arg1, uint32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7);
int32_t sub_904(void);
void sub_928(void* arg1);
void sub_932(int32_t* arg1, int32_t arg2);
int32_t sub_940(int32_t arg1, int32_t arg2);
void sub_97c(void* arg1, int32_t arg2);
void* sub_998(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4);
void sub_a2c(void* arg1, int32_t arg2);
void sub_a4a(void* arg1, char arg2, int32_t arg3);
int32_t sub_a64(int32_t arg1);
int32_t sub_b24(int32_t arg1);
void sub_b44(void* arg1, int32_t arg2);
void sub_b5c(void* arg1, int32_t arg2);
void sub_b78(void* arg1, int32_t arg2);
void sub_b94(int32_t arg1);
int32_t sub_bb8(void* arg1);
int32_t sub_bce(void* arg1, int32_t arg2);
uint32_t sub_be2(void* arg1, int32_t arg2);
int32_t sub_bfe(int32_t* arg1, int32_t arg2);
uint32_t sub_c28(void);
int32_t sub_e1c(void* arg1, char* arg2);
int32_t sub_e70(void);
int32_t sub_e8c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
void sub_ea2(void* arg1);
void sub_eac(void* arg1);
uint32_t sub_eb8(void);
int32_t sub_f64(void);
uint32_t sub_f8c(int32_t* arg1);
uint32_t sub_fca(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_100e(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_1052(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_1098(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_10de(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_1122(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_113e(int32_t arg1, int32_t arg2, int32_t arg3);
uint32_t sub_1224(void);
int32_t sub_12dc(void* arg1, void* arg2, int32_t arg3);
int32_t sub_12fc(void);
int32_t sub_1308(void);
int32_t sub_1330(void);
int32_t sub_133c(void);
int32_t sub_1348(void);
int32_t sub_1354(void);
uint32_t sub_1360(void);
uint32_t sub_1480(void);
int32_t sub_14ac(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_1518(void);
uint32_t sub_1544(void);
uint32_t sub_15a0(void);
int32_t sub_15d0(void);
int32_t sub_1624(void);
int32_t sub_1656(void);
int32_t sub_168e(void);
int32_t sub_16c6(void);
int32_t sub_1700(void);
int32_t sub_173a(void);
int32_t sub_1772(void);
int32_t sub_17ac(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_1808(char arg1, char arg2, char arg3, int32_t arg4);
int32_t sub_1868(void);
void sub_1884(int32_t arg1, void* arg2);
void sub_1888(int32_t arg1, void* arg2);
void sub_188c(int32_t* arg1);
void sub_192c(int32_t* arg1, int32_t arg2);
int32_t sub_1948(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int16_t* arg5);
int32_t sub_1c10(int32_t arg1, int32_t* arg2);
int32_t sub_1c54(int32_t arg1, int32_t* arg2);
void sub_1c70(int32_t* arg1, int32_t* arg2);
void sub_1cb0(int32_t arg1, int32_t arg2, void* arg3, int32_t arg4);
void sub_1cc0(int32_t arg1);
void sub_1cca(int32_t arg1);
int32_t sub_1cd4(void);
int32_t sub_1d30(int32_t arg1);
int32_t sub_1d7c(void);
int32_t sub_1db0(void);
int32_t sub_1de4(int32_t arg1);
int32_t sub_1e30(void);
int32_t sub_1e60(void);
int32_t sub_1ea4(void);
void sub_1ec4(int32_t arg1);
int32_t sub_1ed0(int32_t arg1);
void sub_1ef8(int32_t* arg1);
int32_t sub_1f80(void);
int32_t sub_20dc(int32_t arg1);
uint32_t sub_20f8(void);
int32_t sub_2104(void);
uint32_t sub_2138(void);
uint32_t sub_2144(void);
uint32_t sub_2150(void);
uint32_t sub_219c(void);
uint32_t sub_21d8(void);
int32_t sub_2234(void);
int32_t sub_290c(void);
int32_t sub_29c0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5);
int32_t sub_29dc(void);
int32_t sub_29f2(void);
int32_t sub_2a02(void);
int32_t sub_2a18(void);
int32_t sub_2a28(void);
int32_t sub_2a38(void);
int32_t sub_2a48(void);
int32_t sub_2a66(void);
int32_t sub_2a76(void);
int32_t sub_2ac4(void);
int32_t sub_2ad4(void);
int32_t sub_2af0(void);
int32_t sub_2b3c(int32_t arg1);
void sub_2b60(int32_t arg1, int32_t arg2);
int32_t sub_2ba0(GPIO_Module* arg1, int32_t* arg2);
void sub_2d88(int32_t* arg1);
int32_t sub_2da4(void* arg1, int32_t arg2);
void sub_2db8(GPIO_Module* arg1, int32_t arg2);
void sub_2dbc(GPIO_Module* arg1, int32_t arg2);
int32_t sub_2dc0(void);
int32_t sub_2dd8(void);
int32_t sub_2df0(void);
uint32_t sub_2e08(void);
int32_t sub_2e1c(void);
int32_t sub_2e44(void);
int32_t sub_2e58(void);
void sub_2e8a(void) __attribute__((noreturn));
void sub_2ea8(int32_t arg1);
int32_t sub_2eb4(void);
int32_t sub_2ec4(void);
void sub_2ed4(int32_t arg1);
void sub_2ee0(int32_t arg1);
int32_t sub_2eec(void);
int32_t sub_2efc(void);
int32_t sub_2f0c(void);
int32_t sub_2f64(void);
uint32_t sub_3008(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_3038(void);
int32_t sub_305c(void);
int32_t sub_3084(void);
int32_t sub_30a8(char arg1, char arg2, int32_t arg3, int32_t arg4, int16_t arg5);
int32_t sub_31b4(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_3280(void);
int32_t sub_3358(void);
int32_t sub_3384(void);
int32_t sub_33e8(void);
int32_t sub_3440(void);
int32_t sub_3460(int32_t arg1);
int32_t sub_34bc(int32_t arg1, int16_t arg2);
int32_t sub_34e0(int32_t arg1);
int32_t sub_3500(int32_t arg1);
int32_t sub_3520(char arg1, int32_t arg2, int32_t arg3);
void sub_3558(void* arg1);
void sub_3562(void* arg1);
void sub_356c(void* arg1);
void sub_3576(void* arg1);
int32_t sub_3580(int16_t* arg1);
void sub_3590(void* arg1, int16_t arg2);
void sub_3596(void* arg1, int16_t arg2);
void sub_359c(void* arg1, int32_t arg2);
void sub_35aa(void* arg1, int16_t arg2);
int32_t sub_35b8(void);
int32_t sub_3790(void);
uint32_t sub_37a4(void);
int32_t sub_3810(void);
int32_t sub_3844(void);
int32_t sub_39b0(void);
uint32_t sub_3b1c(void);
uint32_t sub_3b60(void);
int32_t sub_3c80(void);
int32_t sub_3c84(void* arg1);
void sub_3cc8(int32_t arg1, int32_t arg2);
void sub_3d40(void) __attribute__((noreturn));
void sub_3d5c(void) __attribute__((noreturn));
void sub_3d78(void) __attribute__((noreturn));
uint32_t sub_3d94(void);
uint32_t sub_3e78(void);
uint32_t sub_3f84(void);
uint32_t sub_402c(void);
uint32_t sub_4048(void);
int32_t sub_40a4(void);
int32_t sub_40c0(void);
int32_t sub_4120(void);
int32_t sub_4188(void);
void sub_41a0(int32_t arg1);
int32_t sub_41e4(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_4324(void);
uint32_t sub_4328(void);
int32_t sub_43e8(int32_t arg1, int32_t arg2);
int32_t sub_441c(int32_t arg1);
int32_t sub_4438(int32_t arg1, int32_t arg2);
int32_t sub_446c(int32_t arg1);
void sub_448c(int32_t arg1);
void sub_44e4(int32_t arg1);
void sub_450c(int32_t arg1);
int32_t sub_4528(int32_t arg1);
int32_t sub_454c(int32_t arg1);
void sub_456c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
int32_t sub_45c4(int32_t arg1);
int32_t sub_45e0(void);
void sub_463c(int32_t arg1, int32_t arg2);
void sub_465c(int32_t arg1, int32_t arg2);
void sub_467c(int32_t arg1, int32_t arg2);
void sub_469c(int32_t arg1, int32_t arg2);
void sub_46bc(int32_t arg1, int32_t arg2);
void sub_46dc(int32_t arg1, int32_t arg2);
void sub_46fc(int32_t arg1);
void sub_4720(int32_t arg1);
int32_t sub_4748(int32_t arg1);
int32_t sub_4788(void);
int32_t sub_479c(void);
int32_t sub_47d8(void);
int32_t sub_4814(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);
uint32_t sub_4870(void);
uint32_t sub_4884(void);
int32_t sub_48a4(int32_t arg1, int32_t arg2, int32_t arg3);
int32_t sub_4978(int32_t arg1, int32_t arg2, int32_t arg3);
void sub_4a5c(SPI_Module* arg1, int32_t arg2);
int32_t sub_4a78(void);
int32_t sub_4acc(SPI_Module* arg1);
void sub_4b08(SPI_Module* arg1, int32_t arg2, int32_t arg3);
int32_t sub_4b1c(SPI_Module* arg1, int32_t arg2);
uint32_t sub_4b5c(SPI_Module* arg1);
void sub_4b62(SPI_Module* arg1, int16_t arg2);
void sub_4b68(SPI_Module* arg1, int16_t* arg2);
uint32_t sub_4ba8(void);
int32_t sub_4bf4(void);
int32_t sub_4bf8(void);
int32_t sub_4c3c(void);
void sub_4d24(int16_t arg1);
int32_t sub_4d30(void);
int32_t sub_4d38(int16_t arg1);
int32_t sub_4d54(int16_t arg1);
int32_t sub_4d70(void);
uint32_t sub_4f44(void);

/* TIM / peripheral helpers (mapped to TIM1/TIM8 base addresses) */
void sub_59c6(TIM_Module* arg1, int16_t arg2);
void sub_59ca(TIM_Module* arg1, int16_t arg2);
uint32_t sub_59d0(void);
uint32_t sub_59f4(void);

/* Extern data that may be referenced elsewhere */
extern const int data_5555;
extern int data_44[];

#ifdef __cplusplus
}
#endif

#endif
