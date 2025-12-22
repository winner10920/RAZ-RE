#include <stdint.h>
#include <stdbool.h>




int32_t sub_0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4, int32_t arg6 @ r6)
{
    *(arg5 - 0x81 + arg6) = arg4 << 3;
    sub_904();
    /* tailcall */
    return sub_c8();
}

int32_t sub_c8()
{
    /* jump -> 0x8006dd1 */
}

int32_t sub_d8()``
{
    0x80053a5(0x80053a5);
    /* jump -> 0x80000c1 */
}

int32_t sub_f4(int32_t arg1, int32_t arg2) __pure
{
    int32_t r1 = arg1;
    int32_t result = 0;
    int32_t r2 = 0x20;
    
    while (true)
    {
        int32_t r5_6 = r2;
        r2 -= 1;
        
        if (r5_6 <= 0)
            break;
        
        if (r1 >> r2 >= arg2)
        {
            r1 -= arg2 << r2;
            result += 1 << r2;
        }
    }
    
    return result;
}

int32_t sub_120(int32_t arg1, int32_t arg2)
{
    int32_t r4 = 0;
    int32_t r5 = 0;
    
    if (arg1 < 0)
    {
        r4 = 1;
        arg1 = 0 - arg1;
    }
    
    if (arg2 < 0)
    {
        r5 = 1;
        arg2 = 0 - arg2;
    }
    
    int32_t result;
    int32_t r1;
    result = sub_f4(arg1, arg2);
    
    if (r4 != r5)
        result = 0 - result;
    
    return result;
}

void sub_148(int32_t arg1, int32_t arg2)
{
    if (arg1 << 1 >> 1 < arg2 << 1 >> 1)
        int32_t r2_4 = arg1;
}

int32_t sub_1ea(int32_t arg1, int32_t arg2)
{
    /* tailcall */
    return sub_148(arg1, arg2 ^ 0x80000000);
}

int32_t sub_1f2(int32_t arg1, int32_t arg2)
{
    /* tailcall */
    return sub_148(arg1 ^ 0x80000000, arg2);
}

int32_t sub_1fa(int32_t arg1, int32_t arg2) __pure
{
    int32_t result = arg1 << 1;
    
    if (!result)
        return result;
    
    int32_t r2_1 = arg2 << 1;
    
    if (!r2_1)
        return 0;
    
    uint32_t r0_1 = result << 8 >> 9;
    uint32_t r2_3 = r2_1 << 8 >> 9;
    uint32_t r5 = (r0_1 + r2_3) << 7;
    int32_t r4_4 = r2_3 * r0_1 + (r5 << 0x10);
    uint32_t r0_4 = r4_4 >> 0x10;
    int32_t r2_10 = (((~r0_4 + (r2_3 >> 8) * (r0_1 >> 8) + r5) >> 0x10) + 0x4001) << 0x10;
    int32_t r1_2 = (result >> 0x18) + (r2_1 >> 0x18) - 0x7f;
    
    if (r4_4 << 0x10)
        r2_10 += 1;
    
    int32_t r0_5 = r0_4 | r2_10;
    
    if (r0_5 >= 0)
    {
        r0_5 <<= 1;
        r1_2 -= 1;
    }
    
    uint32_t r0_9 = ((r1_2 << 0x18) + (r0_5 >> 7) + 1) >> 1;
    
    if (r0_5 == 0x80)
        r0_9 = r0_9 >> 1 << 1;
    
    if (r1_2 < 0)
        r0_9 = 0;
    
    return r0_9 | (arg1 ^ arg2) >> 0x1f << 0x1f;
}

int32_t sub_274(int32_t arg1, int32_t arg2)
{
    uint32_t r0_1 = arg1 << 1 >> 1;
    uint32_t r2_3 = arg2 << 1 >> 1;
    
    if (r0_1 && r2_3)
    {
        uint32_t i = 0x800000;
        int32_t r1_2 = (r0_1 << 9 >> 9) + 0x800000;
        int32_t r4_3 = (r0_1 >> 0x17) - (r2_3 >> 0x17) + 0x7d;
        
        if (r1_2 < (r2_3 << 9 >> 9) + 0x800000)
            r1_2 <<= 1;
        else
            r4_3 += 1;
        
        if (r4_3 >= 0)
        {
            int32_t r3_2 = 0;
            
            do
            {
                if (r1_2 >= (r2_3 << 9 >> 9) + 0x800000)
                {
                    r1_2 -= (r2_3 << 9 >> 9) + 0x800000;
                    r3_2 |= i;
                }
                
                i u>>= 1;
                r1_2 <<= 1;
            } while (i);
            
            if (r1_2)
            {
                if (r1_2 == (r2_3 << 9 >> 9) + 0x800000)
                    r1_2 = 0x80000000;
                else if (r1_2 >= (r2_3 << 9 >> 9) + 0x800000)
                    r1_2 = 0xfffffffe;
                else
                    r1_2 = 1;
            }
            
            return sub_7c4((r4_3 << 0x17) + r3_2 + ((arg1 ^ arg2) >> 0x1f << 0x1f), r1_2);
        }
    }
    
    return 0;
}

int32_t sub_2f0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r7 = arg1;
    uint32_t r0_2 = (arg2 ^ arg4) >> 0x1f;
    int32_t r6 = arg2;
    uint32_t r0_4 = arg2 << 1 >> 1;
    uint32_t r1_1 = arg4 << 1 >> 1;
    bool c = r7 >= arg3;
    bool cond:1 = ~c;
    
    if (r0_4 - r1_1 >= r0_4 && (r0_4 - r1_1 != r0_4 || !c))
    {
        int32_t r0_6 = r7;
        int32_t r1_2 = r6;
        r6 = arg4;
        r7 = arg3;
        arg4 = r1_2;
        arg3 = r0_6;
    }
    
    if (!(arg4 << 1 >> 1 | arg3))
        /* tailcall */
        return sub_3ba(r7, r6, arg3, arg4);
    
    uint32_t r0_10 = r6 >> 0x14;
    uint32_t r0_12 = r0_10 << 0x15 >> 0x15;
    uint32_t r0_13 = r0_12 - (arg4 << 1 >> 0x15);
    
    if (r0_13 >= 0x40)
    {
        uint32_t r0_39 = r0_2 << 1;
        /* tailcall */
        return sub_418(r7 - r0_2, r6 - (r0_2 >> 0x1f), 1 - r0_39, 0 - (r0_39 >> 0x1f));
    }
    
    int32_t var_2c = arg4 << 0xc >> 0xc | 0x100000;
    int32_t var_30 = arg3;
    
    if (r0_2)
    {
        var_30 = 0 - arg3;
        var_2c = 0 - var_2c;
    }
    
    int32_t r0_21;
    int32_t r1_8;
    r0_21 = sub_75c(var_30, var_2c, 0x40 - r0_13);
    int32_t r5 = r0_21;
    int32_t r4_1 = r1_8;
    int32_t r0_23;
    int32_t r1_10;
    r0_23 = sub_79e(var_30, var_2c, r0_13);
    int32_t r0_24 = r0_23 + r7;
    uint32_t r1_11 = r1_10 + r6;
    
    if ((r1_11 >> 0x14 ^ r0_10) | r0_10 >> 0x1f)
    {
        if (!r0_2)
        {
            r5 = r5 >> 1 | r4_1 << 0x1f;
            r4_1 = r4_1 >> 1 | r0_24 << 0x1f;
            uint32_t r2_18 = r0_10 << 0x14;
            int32_t r1_18 = r1_11 - r2_18 + 0x100000;
            int32_t r0_36 = r0_24 >> 1 | r1_18 << 0x1f;
            r0_24 = r0_36;
            r1_11 = (r1_18 >> 1) + r2_18;
        }
        else
        {
            if (r0_13 <= 1)
            {
                uint32_t r2_8 = r0_10 << 0x14;
                int32_t r0_27;
                int32_t r1_14;
                int32_t r2_12;
                int32_t r3_1;
                r0_27 = sub_860(r0_24, r1_11 - r2_8 + 0x100000, r5, r4_1, 0, r2_8 >> 0x1f << 0x1f, 
                    r0_12);
                /* tailcall */
                return sub_3ba(r0_27, r1_14, r2_12, r3_1);
            }
            
            uint32_t r6_5 = r0_10 << 0x14;
            int32_t r7_4 = r1_11 - r6_5 + 0x100000;
            int32_t r0_31 = r0_24 * 2;
            r1_11 = r6_5 - 0x200000 + r7_4 + r7_4;
            r0_24 = r0_31 | r4_1 >> 0x1f;
            int32_t temp14_1 = r5;
            int32_t temp15_1 = r5;
            r5 *= 2;
            r4_1 = r4_1 + r4_1;
        }
    }
    
    /* tailcall */
    return sub_418(r0_24, r1_11, r5, r4_1);
}

int32_t sub_3ba(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __pure
{
    return;
}

int32_t sub_418(uint32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r0;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    r0 = sub_846(arg1, arg2, arg3, arg4);
    /* tailcall */
    return sub_3ba(r0, r1, r2, r3);
}

int32_t sub_41e(int32_t arg1 @ r6, int32_t arg2 @ r7, int32_t arg3)
{
    int32_t r0_1 = arg3 << 1;
    /* tailcall */
    return sub_418(arg2 - arg3, arg1 - (arg3 >> 0x1f), 1 - r0_1, 0 - (r0_1 >> 0x1f));
}

int32_t sub_43a(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    /* tailcall */
    return sub_2f0(arg1, arg2, arg3, arg4 ^ 0x80000000);
}

int32_t sub_446(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    /* tailcall */
    return sub_2f0(arg1, arg2 ^ 0x80000000, arg3, arg4);
}

int32_t sub_458(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_14 = r4;
    int32_t var_18 = r4;
    int32_t var_20 = arg3;
    int32_t var_28 = arg1;
    uint32_t r1_3 = arg2 << 1 >> 1;
    uint32_t r1_6 = arg4 << 1 >> 1;
    
    if (var_28 | r1_3 && var_20 | r1_6)
    {
        int32_t var_24_2 = r1_3 << 0xc >> 0xc | 0x100000;
        int32_t var_1c_2 = r1_6 << 0xc >> 0xc | 0x100000;
        int32_t r2_1 = 0;
        uint32_t r4_1 = 0;
        int32_t r0_21 = 0;
        
        while (true)
        {
            int32_t r3 = r0_21;
            
            while (true)
            {
                int32_t r12_1 = r3;
                int32_t r1_11 = r0_21 - r3;
                
                while (r1_11 <= r12_1)
                {
                    int32_t r5_2 = *(&var_20 + (r1_11 << 1)) * *(&var_28 + (r3 << 1));
                    int32_t temp0_1 = r2_1;
                    r2_1 += r5_2;
                    r4_1 = r4_1 + 0;
                    r1_11 += 1;
                    r3 -= 1;
                }
                
                int32_t var_38;
                *(&var_38 + (r0_21 << 1)) = r2_1;
                r2_1 = r2_1 >> 0x10 | r4_1 << 0x10;
                r4_1 u>>= 0x10;
                r0_21 += 1;
                int32_t var_34;
                uint32_t var_30;
                uint32_t var_2c;
                
                if (r0_21 >= 8)
                    return sub_860(var_30, var_2c, var_38, var_34, 0, 
                        (arg2 ^ arg4) >> 0x1f << 0x1f, 
                        (r1_3 << 1 >> 0x15) + (r1_6 << 1 >> 0x15) - 0x3f3);
                
                if (r0_21 < 3)
                    break;
                
                r3 = 3;
            }
        }
    }
    
    return 0;
}

int32_t sub_528(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    uint32_t r4_5 = arg2 << 1 >> 1;
    uint32_t r1_1 = arg4 << 1 >> 1;
    
    if (arg1 | r4_5 && arg3 | r1_1)
    {
        int32_t r2_6 = arg1;
        int32_t r3_2 = r4_5 << 0xc >> 0xc | 0x100000;
        int32_t r6_3 = r1_1 << 0xc >> 0xc | 0x100000;
        int32_t var_28_1 = (r4_5 << 1 >> 0x15) - (r1_1 << 1 >> 0x15) + 0x3fd;
        bool c_1 = r2_6 >= arg3;
        bool cond:0_1 = ~c_1;
        int32_t r0_4;
        
        if (r3_2 - r6_3 >= r3_2 && (r3_2 - r6_3 != r3_2 || !c_1))
        {
            int32_t temp1_1 = r2_6;
            int32_t temp2_1 = r2_6;
            r2_6 *= 2;
            r0_4 = var_28_1;
            r3_2 = r3_2 + r3_2;
        }
        else
        {
            r0_4 = var_28_1 + 1;
            var_28_1 = r0_4;
        }
        
        if (r0_4 >= 0)
        {
            int32_t r0_6 = 0;
            uint32_t r1_4 = 0x100000;
            int32_t r7_2 = 0;
            int32_t lr_1 = 0;
            
            while (r0_6 | r1_4)
            {
                bool c_4 = r2_6 >= arg3;
                bool cond:1_1 = ~c_4;
                
                if (r3_2 - r6_3 < r3_2 || (r3_2 - r6_3 == r3_2 && c_4))
                {
                    int32_t temp7_1 = r2_6;
                    r2_6 -= arg3;
                    r3_2 = r3_2 - r6_3;
                    r7_2 |= r0_6;
                    lr_1 |= r1_4;
                }
                
                int32_t r5_5 = r1_4 << 0x1f;
                r1_4 u>>= 1;
                r0_6 = r0_6 >> 1 | r5_5;
                int32_t temp5_1 = r2_6;
                int32_t temp6_1 = r2_6;
                r2_6 *= 2;
                r3_2 = r3_2 + r3_2;
            }
            
            if (r2_6 | r3_2)
            {
                if (!((arg3 ^ r2_6) | (r3_2 ^ r6_3)))
                {
                    r2_6 = 0;
                    r3_2 = 0x80000000;
                }
                else
                {
                    bool c_8 = r2_6 >= arg3;
                    bool cond:2_1 = ~c_8;
                    
                    if (r3_2 - r6_3 < r3_2 || (r3_2 - r6_3 == r3_2 && c_8))
                    {
                        r2_6 = 0xfffffffe;
                        r3_2 = 0xffffffff;
                    }
                    else
                    {
                        r2_6 = 1;
                        r3_2 = 0;
                    }
                }
            }
            
            return sub_846(r7_2, (var_28_1 << 0x14) + lr_1 + ((arg2 ^ arg4) >> 0x1f << 0x1f), r2_6, 
                r3_2);
        }
    }
    
    return 0;
}

int32_t sub_618(uint32_t arg1)
{
    int32_t r3;
    int32_t var_8 = r3;
    int32_t r2;
    int32_t var_c = r2;
    int32_t r1;
    int32_t var_10 = r1;
    return sub_860(arg1, 0, 0, 0, 0, 0, 0x433);
}

int32_t sub_634(int32_t arg1) __pure
{
    uint32_t r1 = arg1 << 1 >> 0x18;
    
    if (r1 < 0x7f)
        return 0;
    
    if (r1 > 0x96)
        return ((arg1 << 9 >> 9) + 0x800000) << (r1 - 0x96);
    
    return ((arg1 << 9 >> 9) + 0x800000) >> (0x96 - r1);
}

int32_t sub_65c(int32_t arg1, int32_t arg2)
{
    uint32_t r2 = arg2 << 1 >> 0x15;
    
    if (r2 < 0x3ff)
        return 0;
    
    if (r2 > 0x433)
        return arg1 << (r2 - 0x433);
    
    return sub_77c(arg1, arg2 << 0xc >> 0xc | 0x100000, 0x433 - r2);
}

uint32_t sub_698(int32_t arg1) __pure
{
    uint32_t r0_1 = arg1 << 1 >> 1;
    
    if (!r0_1)
        return 0;
    
    return r0_1 << 9 >> 9 << 0x1d;
}

void sub_6c0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __pure
{
    return;
}

void sub_6e8(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __pure
{
    return;
}

int32_t sub_710(int32_t arg1, int32_t arg2)
{
    uint32_t r1_1 = arg2 << 1 >> 1;
    
    if (arg1 | r1_1 && (r1_1 >> 0x14) - 0x380 > 0)
        return sub_7c4(
            (arg1 >> 0x1d | r1_1 << 0xc >> 0xc << 3) + (arg2 >> 0x1f << 0x1f)
                + (((r1_1 >> 0x14) - 0x380) << 0x17), 
            arg1 << 3);
    
    return 0;
}

void sub_748(int32_t arg1, int32_t arg2) __pure
{
    return;
}

int32_t sub_75c(int32_t arg1, int32_t arg2, int32_t arg3) __pure
{
    if (arg3 < 0x20)
        return arg1 << arg3;
    
    return 0;
}

int32_t sub_77c(int32_t arg1, int32_t arg2, int32_t arg3) __pure
{
    if (arg3 < 0x20)
        return arg1 >> arg3 | arg2 << (0x20 - arg3);
    
    return arg2 >> (arg3 - 0x20);
}

int32_t sub_79e(int32_t arg1, int32_t arg2, int32_t arg3) __pure
{
    if (arg3 < 0x20)
        return arg1 >> arg3 | arg2 << (0x20 - arg3);
    
    return arg2 >> (arg3 - 0x20);
}

void sub_7c4(uint32_t arg1, int32_t arg2) __pure
{
    return;
}

int32_t sub_7d4(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __pure
{
    int32_t r4 = 0;
    
    if (arg1 >> 0x10)
        goto label_7ea;
    
    r4 = 0x10;
    arg1 <<= 0x10;
    
    if (arg1)
        goto label_7ea;
    
    if (arg2)
    {
        r4 = 0x11;
    label_7ea:
        
        if (!(arg1 >> 0x18))
        {
            arg1 <<= 8;
            r4 += 8;
        }
        
        if (!(arg1 >> 0x1c))
        {
            arg1 <<= 4;
            r4 += 4;
        }
        
        if (!(arg1 >> 0x1e))
        {
            arg1 <<= 2;
            r4 += 2;
        }
        
        if (arg1 >= 0)
        {
            arg1 <<= 1;
            r4 += 1;
        }
        
        if (arg2)
        {
            int32_t r1 = arg2 << r4;
            
            if (r1)
                r1 = 1;
            
            arg1 |= r1 | arg2 >> (0x20 - r4);
        }
        
        int32_t r1_2 = arg1 << 0x18;
        int32_t r3 = arg4 - r4;
        
        if (r3 + 7 < 0)
            return 0;
        
        arg1 = ((r3 + 7) << 0x17) + (arg1 >> 8) + arg3;
        
        if (r1_2 < 0)
        {
            arg1 += 1;
            
            if (!(r1_2 << 1))
                return arg1 >> 1 << 1;
        }
    }
    
    return arg1;
}

void sub_846(uint32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __pure
{
    return;
}

int32_t sub_860(uint32_t arg1, uint32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7)
{
    int32_t r4;
    int32_t var_14 = r4;
    int32_t var_18 = r4;
    uint32_t var_24 = arg2;
    uint32_t var_28 = arg1;
    int32_t r0_1;
    
    if (!arg2)
        r0_1 = sub_5d48(arg1) + 0x20;
    else
        r0_1 = sub_5d48(arg2);
    
    int32_t result_2;
    int32_t r1_1;
    result_2 = sub_75c(arg1, arg2, r0_1);
    int32_t result_1 = result_2;
    int32_t r7 = r1_1;
    int32_t result;
    
    if (!(result_2 | arg3 | r1_1 | arg4))
        result = result_1;
    else
    {
        if (arg3 | arg4)
        {
            int32_t r0_10;
            int32_t r1_4;
            r0_10 = sub_77c(arg3, arg4, 0x40 - r0_1);
            int32_t r0_12;
            int32_t r1_6;
            r0_12 = sub_75c(arg3, arg4, r0_1);
            int32_t r0_14;
            
            r0_14 = !(r0_12 | r1_6) ? 0 : 1;
            
            result_1 |= r0_10 | r0_14;
            r7 |= r1_4 | r0_14 >> 0x1f;
        }
        
        int32_t r0_16 = arg7 - r0_1;
        int32_t r4_4 = result_1 >> 0xb | r7 << 0x15;
        
        if (r0_16 + 0xa >= 0)
            return sub_846(r4_4 + arg5, ((r0_16 + 0xa) << 0x14) + (r7 >> 0xb) + arg6, 0, 
                result_1 << 0x15);
        
        result = 0;
    }
    
    return result;
}

int32_t sub_904()
{
    for (int32_t* i = 0x800939c; i < 0x80093bc; i = &i[4])
        (i[3] | 1)(*i, i[1], i[2]);
    
    int32_t r0_2;
    int32_t r3_3;
    r0_2 = sub_c8();
    int32_t arg_270 = r3_3;
    int32_t arg_2f0 = r3_3;
    /* tailcall */
    return sub_928(0);
}

void sub_928(void* arg1)
{
    *(arg1 + 0x54) &= 0xffffffef;
}

void sub_932(int32_t* arg1, int32_t arg2)
{
    *arg1 = 0x7f & ~(arg2 >> 8);
}

int32_t sub_940(int32_t arg1, int32_t arg2)
{
    if (arg1)
    {
        sub_4438(arg2, 1);
        sub_441c(0);
        sub_ea2(0x40020800);
    }
    else
    {
        sub_4438(0x100, 0);
        sub_441c(arg2);
        sub_928(0x40020800);
    }
    
    return 0x40020800;
}

void sub_97c(void* arg1, int32_t arg2)
{
    *(arg1 + 8) = (*(arg1 + 8) & 0xffff8fff) | arg2;
}

void* sub_998(void* arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t var_1c = arg3;
    int32_t var_20 = arg2;
    
    if (arg2 <= 7)
        *(arg1 + 0x14) = (*(arg1 + 0x14) & ~(0xf << arg2 << 2)) | arg4 << arg2 << 2;
    else
        *(arg1 + 0x10) = (*(arg1 + 0x10) & ~(0xf << (arg2 - 8) << 2)) | arg4 << (arg2 - 8) << 2;
    
    int32_t r0_7 = *(arg1 + 0x3c);
    uint32_t r3 = (0x300000 & r0_7) >> 0x14;
    *(arg1 + 0x3c) =
        (r0_7 & ~(0xf << ((arg3 + 3 - (r3 + 1)) * 5))) | arg2 << ((arg3 + 3 - (r3 + 1)) * 5);
    return arg1;
}

void sub_a2c(void* arg1, int32_t arg2)
{
    *(arg1 + 0x3c) = (*(arg1 + 0x3c) & 0xffcfffff) | (arg2 - 1) << 0x14;
}

void sub_a4a(void* arg1, char arg2, int32_t arg3)
{
    uint32_t r3 = arg2;
    
    if (!arg3)
        *(arg1 + 4) &= ~r3;
    else
        *(arg1 + 4) |= r3;
}

int32_t sub_a64(int32_t arg1)
{
    int32_t r3;
    int32_t var_c = r3;
    int32_t r2;
    int32_t var_10_1 = r2;
    int32_t r1;
    int32_t var_14_1 = r1;
    int32_t result = arg1;
    result = 1;
    *result[1] = 1;
    int32_t var_14 = 0xe0000;
    int32_t var_10 = 0;
    var_c = 4;
    sub_e1c(0x40020800, &result);
    sub_a2c(0x40020800, 4);
    sub_998(0x40020800, 1, 1, 9);
    sub_998(0x40020800, 2, 2, 9);
    sub_998(0x40020800, 0xa, 3, 9);
    sub_998(0x40020800, 0xd, 4, 9);
    sub_97c(0x40020800, 0x7000);
    sub_b94(1);
    sub_a4a(0x40020800, 0x80, 1);
    sub_b5c(0x40020800, 1);
    sub_b44(0x40020800, 1);
    int32_t i;
    
    do
        i = sub_bce(0x40020800, 0x20);
     while (!i);
    int32_t i_1;
    
    do
        i_1 = sub_bce(0x40020800, 0x40);
     while (i_1);
    sub_eac(0x40020800);
    int32_t i_2;
    
    do
        i_2 = sub_bb8(0x40020800);
     while (i_2);
    return result;
}

int32_t sub_b24(int32_t arg1)
{
    int32_t result = 0x40020800;
    
    if (arg1 == 0x40020800)
    {
        sub_465c(0x1000, 1);
        result = arg1 >> 0x12;
        sub_465c(result, 0);
    }
    
    return result;
}

void sub_b44(void* arg1, int32_t arg2)
{
    if (!arg2)
        *(arg1 + 8) = *(arg1 + 8) >> 1 << 1;
    else
        *(arg1 + 8) |= 1;
}

void sub_b5c(void* arg1, int32_t arg2)
{
    if (!arg2)
        *(arg1 + 4) &= 0xfffffbff;
    else
        *(arg1 + 4) |= 0x400;
}

void sub_b78(void* arg1, int32_t arg2)
{
    if (!arg2)
        *(arg1 + 8) &= 0xffdf7fff;
    else
        *(arg1 + 8) |= 0x208000;
}

void sub_b94(int32_t arg1)
{
    if (!arg1)
        *0x40020854 &= 0xfffffffd;
    else
        *0x40020854 |= 2;
}

int32_t sub_bb8(void* arg1)
{
    if (!(*(arg1 + 8) & 4))
        return 0;
    
    return 1;
}

int32_t sub_bce(void* arg1, int32_t arg2)
{
    if (!(*(arg1 + 0x54) & arg2))
        return 0;
    
    return 1;
}

uint32_t sub_be2(void* arg1, int32_t arg2)
{
    int32_t r3;
    int32_t var_8_3 = r3;
    int32_t var_8 = 0;
    return *(arg1 + arg2 + 0x28);
}

int32_t sub_bfe(int32_t* arg1, int32_t arg2)
{
    if (*arg1 & arg2 >> 8 && arg1[1] & arg2)
        return 1;
    
    return 0;
}

uint32_t sub_c28()
{
    uint32_t result = sub_bfe(0x40020800, 0x480);
    bool cond:0 = result != 1;
    
    while (!cond:0)
    {
        sub_932(0x40020800, 0x480);
        *0x20000030 = sub_be2(0x40020800, 0x18);
        *0x20000032 = sub_be2(0x40020800, 0x1c);
        *0x20000034 = sub_be2(0x40020800, 0x20);
        *0x20000036 = sub_be2(0x40020800, 0x24);
        *0x2000012c = sub_120(0x1d8a8, *0x20000036);
        *0x20000130 = *0x2000012c * (*0x20000034 << 1);
        result = *0x20000019 << 0x1b >> 0x1f;
        
        if (!result)
            break;
        
        if (*0x20000150)
        {
            if (*0x20000040 > 0x5f370)
            {
                result = *0x20000130;
                
                if (result <= 0x4ce78)
                {
                    sub_1518();
                    *0x20000019 &= 0xdf;
                    result = *0x20000006;
                    
                    if (result == 3)
                    {
                        *0x20000006 = 8;
                        return sub_72e8();
                    }
                }
            }
            else if (*0x20000040 <= 0x5a550)
            {
                result = *0x20000130;
                
                if (result <= 0x45948)
                {
                    sub_1518();
                    *0x20000019 &= 0xdf;
                    result = *0x20000006;
                    
                    if (result == 3)
                    {
                        *0x20000006 = 8;
                        return sub_72e8();
                    }
                }
            }
            else
            {
                result = *0x20000130;
                
                if (result <= 0x46cd0)
                {
                    sub_1518();
                    *0x20000019 &= 0xdf;
                    result = *0x20000006;
                    
                    if (result == 3)
                    {
                        *0x20000006 = 8;
                        return sub_72e8();
                    }
                }
            }
            
            break;
        }
        
        if (*0x20000040 <= 0x5f370)
        {
            if (*0x20000040 <= 0x5a550)
            {
                result = *0x20000130;
                
                if (result <= 0x46cd0)
                {
                    sub_1518();
                    *0x20000019 &= 0xdf;
                    result = *0x20000006;
                    
                    if (result == 3)
                    {
                        *0x20000006 = 8;
                        return sub_72e8();
                    }
                }
            }
            else
            {
                result = *0x20000130;
                
                if (result <= 0x493e0)
                {
                    sub_1518();
                    *0x20000019 &= 0xdf;
                    result = *0x20000006;
                    
                    if (result == 3)
                    {
                        *0x20000006 = 8;
                        return sub_72e8();
                    }
                }
            }
            
            break;
        }
        
        result = *0x20000130;
        
        if (result > 0x4baf0)
            break;
        
        sub_1518();
        *0x20000019 &= 0xdf;
        result = *0x20000006;
        cond:0 = result != 3;
        
        if (result == 3)
        {
            *0x20000006 = 8;
            return sub_72e8();
        }
    }
    
    return result;
}

int32_t sub_e1c(void* arg1, char* arg2)
{
    *(arg1 + 4) = (*(arg1 + 4) & 0xfffffeff) | *arg2 << 8;
    *(arg1 + 8) = (*(arg1 + 8) & 0xfff1f7fd) | *(arg2 + 8) | *(arg2 + 4) | arg2[1] << 1;
    int32_t result = (*(arg1 + 0x30) & 0xff0fffff) | (arg2[0xc] - 1) << 0x14;
    *(arg1 + 0x30) = result;
    return result;
}

int32_t sub_e70()
{
    int32_t r0;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    r0 = sub_3038();
    sub_a64(sub_e8c(r0, r1, r2, r3));
    sub_b78(0x40020800, 1);
    return 0x40020800;
}

int32_t sub_e8c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t var_8 = arg4;
    var_8 = 0x15;
    *var_8[1] = 0;
    *var_8[2] = 1;
    return sub_3c84(&var_8);
}

void sub_ea2(void* arg1)
{
    *(arg1 + 0x54) |= 0x10;
}

void sub_eac(void* arg1)
{
    *(arg1 + 8) |= 4;
}

uint32_t sub_eb8()
{
    uint32_t result;
    
    if (!(*0x20000019 << 0x1a >> 0x1f))
    {
        if (*0x200000d0 != 1)
        {
            if (*0x200000d4 >= 0x2710)
                *0x20000121 = *0x20000120;
            else
                *0x200000d4 += 1;
            
            sub_1224();
            return sub_779c();
        }
        
        *0x200000d2 += 1;
        result = *0x200000d2;
        
        if (result > 0x3e8)
        {
            *0x200000d0 = 0;
            *0x200000c9 = 0;
            *0x200000cc = 0x44c;
            result = 0;
            *0x2000012a = 0;
            *0x200000d2 = 0;
        }
    }
    else
    {
        *0x200000d0 = 1;
        result = 0;
        *0x200000d2 = 0;
        *0x200000d4 = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

int32_t sub_f64()
{
    sub_65e0(1);
    *0x2000013c = sub_305c();
    *0x200009bc = *0x2000013c;
    return sub_5c50();
}

uint32_t sub_f8c(int32_t* arg1 @ r5)
{
    int32_t* var_c = arg1;
    uint32_t r0 = *0x20000120;
    int32_t r0_1;
    int32_t entry_r1;
    int32_t r1;
    int32_t entry_r2;
    int32_t r2;
    int32_t r3_1;
    r0_1 = sub_5d76(r0, entry_r1, entry_r2, r0);
    *(r3_1 - 0x17) = arg1;
    *arg1 = r0_1;
    arg1[1] = r1;
    arg1[2] = arg1;
    arg1[3] = r0_1 << 0x14;
    uint32_t r0_3;
    int32_t r1_1;
    r0_3 = sub_698(*0x2000013c);
    
    if (!TEST_BIT(r2, 0x1d) && !(*0x40010c10 & 2))
        *0x20000120 = 1;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_fca(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (c)
    {
        uint32_t r0_5;
        int32_t r1_1;
        r0_5 = sub_698(*0x2000013c);
        
        if (!c)
            *0x20000120 = 0;
    }
    else if (!(*0x40010c10 & 2))
        *0x20000120 = 2;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_100e(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (c)
    {
        uint32_t r0_5;
        int32_t r1_1;
        r0_5 = sub_698(*0x2000013c);
        
        if (!c)
            *0x20000120 = 1;
    }
    else if (!(*0x40010c10 & 2))
        *0x20000120 = 3;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_1052(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (c)
    {
        uint32_t r0_5;
        int32_t r1_1;
        r0_5 = sub_698(*0x2000013c);
        *0x20000120 = 2;
    }
    else if (!(*0x40010c10 & 2))
        *0x20000120 = 4;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_1098(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (c)
    {
        uint32_t r0_5;
        int32_t r1_1;
        r0_5 = sub_698(*0x2000013c);
        *0x20000120 = 3;
    }
    else if (!(*0x40010c10 & 2))
        *0x20000120 = 5;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_10de(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (c)
    {
        uint32_t r0_5;
        int32_t r1_1;
        r0_5 = sub_698(*0x2000013c);
        
        if (!c)
            *0x20000120 = 4;
    }
    else if (!(*0x40010c10 & 2))
        *0x20000120 = 6;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_1122(int32_t arg1, int32_t arg2, int32_t arg3)
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    bool c;
    
    if (!c)
        *0x20000120 = 5;
    
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_113e(int32_t arg1, int32_t arg2, int32_t arg3)
{
    if (*0x20000120 != *0x200000c8)
    {
        *0x200000c8 = *0x20000120;
        
        if (sub_2e44() && *0x40010c10 & 2 && *0x20000006 != 1 && *0x20000006 != 2
                && *0x20000006 != 4 && *0x20000006 != 5 && *0x20000006 != 3 && *0x20000006 != 6
                && *0x20000006 != 0xe)
            sub_1de4(*0x20000120);
    }
    
    sub_2e58();
    uint32_t result = *0x20000120;
    
    if (result > 6)
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

uint32_t sub_1224()
{
    if (!*0x200000c9)
    {
        *0x200000cc += 1;
        
        if (*0x200000cc > 0x44c)
        {
            *0x200000c9 = 1;
            sub_1480();
            *0x200000cc = 0;
        }
    }
    else if (*0x200000c9 == 1)
    {
        if (*0x200000cc <= 3)
        {
            *0x200009bc = sub_305c();
            *(0x2000098c + (*0x200000cc << 2)) = *0x200009bc;
        }
        else
        {
            *0x200009c0 = sub_274(sub_148(*0x20000994, *0x20000998), 0x40000000);
            *0x2000013c = *0x200009c0;
            sub_1624();
            *0x200000c9 = 0;
            sub_402c();
            *0x200000cc = 0;
        }
        
        *0x200000cc += 1;
    }
    
    uint32_t result = *0x200000c9;
    
    if (result)
        return result;
    
    int32_t* r5;
    return sub_f8c(r5);
}

int32_t sub_12dc(void* arg1, void* arg2, int32_t arg3)
{
    int32_t r1 = 0;
    
    while (true)
    {
        if (r1 >= arg3)
            return 0;
        
        if (*(arg1 + r1) != *(arg2 + r1))
            break;
        
        r1 += 1;
    }
    
    return 1;
}

int32_t sub_12fc()
{
    *0x20000164 = 0;
    return 0;
}

int32_t sub_1308()
{
    *0x2000005e = 0;
    *0x2000001c &= 0xbf;
    *0x2000011a = 0;
    return 0;
}

int32_t sub_1330()
{
    *0x20000027 = 0;
    return 0;
}

int32_t sub_133c()
{
    *0x20000026 = 0;
    return 0;
}

int32_t sub_1348()
{
    *0x20000050 = 0;
    return 0;
}

int32_t sub_1354()
{
    *0x2000004c = 0;
    return 0;
}

uint32_t sub_1360()
{
    uint32_t result = *0x2000001c << 0x19 >> 0x1f;
    
    if (result)
    {
        if (*0x200000d8)
        {
            result = *0x200000d8;
            
            if (result == 1)
            {
                result = *0x2000012a;
                
                if (result > 0x12b)
                {
                    int32_t r0_13 = *0x2000012a - 0x12b;
                    uint32_t r4_2 = ((r0_13 >> 0x1f) + r0_13) << 0x17 >> 0x18;
                    
                    if (r4_2 > 0x64)
                        r4_2 = 0x64;
                    
                    *0x2000005e = 0x64 - r4_2;
                    result = *0x2000005f;
                    
                    if (result > *0x2000005e)
                    {
                        *0x2000005f = *0x2000005e;
                        *0x2000011a = 0xfa0 - r4_2 * 0x28;
                        *0x20000140 = 0xfa0 - r4_2 * 0x28;
                        sub_59c6(0x40000400, *0x2000011a);
                        result = *0x2000011a;
                        
                        if (!result)
                        {
                            result = *0x2000001c & 0xffffffbf;
                            *0x2000001c = result;
                        }
                    }
                }
            }
        }
        else
        {
            result = *0x2000012a;
            
            if (result > 0x12b)
            {
                int32_t r0_5 = *0x2000012a - 0x12b;
                uint32_t r4_1 = ((r0_5 >> 0x1f) + r0_5) << 0x17 >> 0x18;
                
                if (r4_1 > 0x64)
                    r4_1 = 0x64;
                
                *0x2000005e = 0x64 - r4_1;
                result = *0x2000005f;
                
                if (result > *0x2000005e)
                {
                    *0x2000005f = *0x2000005e;
                    *0x2000011a = 0x4b0 - r4_1 * 0xc;
                    *0x20000140 = 0x4b0 - r4_1 * 0xc;
                    sub_59c6(0x40000400, *0x2000011a);
                    result = *0x2000011a;
                    
                    if (!result)
                    {
                        result = *0x2000001c & 0xffffffbf;
                        *0x2000001c = result;
                    }
                }
            }
        }
    }
    
    return result;
}

uint32_t sub_1480()
{
    uint32_t result = *0x20000068;
    
    if (result == 1)
    {
        result = *0x40010c10 & 2;
        
        if (!result)
        {
            sub_639c();
            result = 0;
            *0x20000068 = 0;
        }
    }
    
    return result;
}

int32_t sub_14ac(int32_t arg1, int32_t arg2, int32_t arg3)
{
    int32_t r3;
    int32_t var_8 = r3;
    int32_t var_c = arg3;
    int32_t var_10 = arg2;
    var_c = 0x8a;
    var_8 = 0;
    *var_8[1] = 0xc;
    *var_8[2] = 0;
    sub_1ef8(&var_c);
    sub_2b60(1, 1);
    var_10 = 6;
    *var_10[1] = 1;
    *var_10[2] = 0;
    sub_3c84(&var_10);
    sub_2b60(0, 3);
    var_10 = 7;
    *var_10[1] = 1;
    *var_10[2] = 0;
    sub_3c84(&var_10);
    sub_2b60(0, 7);
    var_10 = 8;
    *var_10[1] = 1;
    *var_10[2] = 0;
    return sub_3c84(&var_10);
}

int32_t sub_1518()
{
    sub_59ca(0x40012c00, 0);
    sub_59c6(0x40013400, 0);
    sub_402c();
    *0x20000128 = 0;
    return 0;
}

uint32_t sub_1544()
{
    uint32_t result = *0x20000019 << 0x1b >> 0x1f;
    
    if (!result)
    {
        if (*0x20000038 <= 7)
        {
            *(0x20000168 + (*0x20000038 << 1)) = *0x20000034;
            result = *0x20000038 + 1;
            *0x20000038 = result;
        }
        else
        {
            *0x20000038 = 0;
            *(0x20000168 + (*0x20000038 << 1)) = *0x20000034;
            result = *0x20000038 + 1;
            *0x20000038 = result;
        }
    }
    
    return result;
}

uint32_t sub_15a0()
{
    if (*0x20000144 < 0x53020)
        return 6 - sub_f4(*0x20000144, 0xea60);
    
    return 0;
}

int32_t sub_15d0()
{
    int32_t var_18 = 0;
    int32_t var_10 = 0;
    int32_t var_18_1 = 0x10110000;
    int32_t var_1c = 8;
    sub_2ba0(0x40010800, &var_1c);
    int32_t var_24 = 8;
    char var_20 = 0;
    char var_1f = 0x10;
    char var_1e = 1;
    sub_1ef8(&var_24);
    sub_2b60(0, 3);
    char var_28 = 7;
    char var_26 = 1;
    return sub_3c84(&var_28);
}

int32_t sub_1624()
{
    uint32_t r0 = *0x20000120;
    int32_t r0_1;
    int32_t entry_r1;
    void* r1;
    int32_t entry_r2;
    r0_1 = sub_5d76(r0, entry_r1, entry_r2, r0);
    *(r1 + 4);
    uint32_t r0_3;
    int32_t r1_1;
    r0_3 = sub_698(*0x2000013c);
    int32_t r0_4;
    int32_t r1_2;
    int32_t r2;
    r0_4 = sub_6e8(r0_3, r1_1, 0x851eb852, 0x400c51eb);
    
    if (!TEST_BIT(r0_1 << 0x14, 0x1e))
        *0x20000120 = 1;
    
    /* tailcall */
    return sub_17ac(r0_4, r1_2, r2);
}

int32_t sub_1656()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6e8(r0_1, r1, 0x51eb851f, 0x400d1eb8);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0, 0x400c0000);
        
        if (!c)
            *0x20000120 = 0;
    }
    else
        *0x20000120 = 2;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t sub_168e()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6e8(r0_1, r1, 0x1eb851ec, 0x400deb85);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0xcccccccd, 0x400ccccc);
        
        if (!c)
            *0x20000120 = 1;
    }
    else
        *0x20000120 = 3;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t sub_16c6()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6e8(r0_1, r1, 0x66666666, 0x400e6666);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0x9999999a, 0x400d9999);
        *0x20000120 = 2;
    }
    else
        *0x20000120 = 4;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t sub_1700()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6e8(r0_1, r1, 0x70a3d70a, 0x400f0a3d);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0xe147ae14, 0x400e147a);
        *0x20000120 = 3;
    }
    else
        *0x20000120 = 5;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t sub_173a()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6e8(r0_1, r1, 0x3d70a3d7, 0x400fd70a);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0xeb851eb8, 0x400eb851);
        
        if (!c)
            *0x20000120 = 4;
    }
    else
        *0x20000120 = 6;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t sub_1772()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t r0_2;
    int32_t r1_1;
    int32_t r2;
    r0_2 = sub_6c0(r0_1, r1, 0, 0x400c0000);
    bool c;
    
    if (c)
    {
        uint32_t r0_4;
        int32_t r1_2;
        r0_4 = sub_698(*0x2000013c);
        r0_2 = sub_6c0(r0_4, r1_2, 0xb851eb85, 0x400f851e);
        
        if (!c)
            *0x20000120 = 5;
    }
    else
        *0x20000120 = 0;
    
    /* tailcall */
    return sub_17ac(r0_2, r1_1, r2);
}

int32_t j_sub_17ac(int32_t arg1, int32_t arg2, int32_t arg3)
{
    /* tailcall */
    return sub_17ac(arg1, arg2, arg3);
}

int32_t sub_17ac(int32_t arg1, int32_t arg2, int32_t arg3) __pure
{
    return;
}

int32_t sub_1808(char arg1, char arg2, char arg3, int32_t arg4)
{
    int32_t var_18 = arg4;
    sub_34e0(0x2a);
    sub_3500(0);
    sub_3500(arg1);
    sub_3500(0);
    sub_3500(arg1 + arg3 - 1);
    sub_34e0(0x2b);
    sub_3500(0);
    sub_3500(arg2);
    sub_3500(0);
    sub_3500(arg2 + arg4 - 1);
    return sub_34e0(0x2c);
}

int32_t sub_1868()
{
    int32_t result = sub_1c54(0x200, 0x40020000);
    
    if (result)
    {
        result = 0x200;
        sub_1888(0x200, 0x40020000);
    }
    
    return result;
}

void sub_1884(int32_t arg1, void* arg2)
{
    *(arg2 + 4) = arg1;
}

void sub_1888(int32_t arg1, void* arg2)
{
    *(arg2 + 4) = arg1;
}

void sub_188c(int32_t* arg1)
{
    *arg1 &= 0xfffe;
    *arg1 = 0;
    arg1[1] = 0;
    arg1[2] = 0;
    arg1[3] = 0;
    
    if (arg1 == 0x40020008)
        *0x40020004 |= 0xf;
    else if (arg1 == 0x4002001c)
        *0x40020004 |= 0xf0;
    else if (arg1 == 0x40020030)
        *0x40020004 |= 0xf00;
    else if (arg1 == 0x40020044)
        *0x40020004 |= 0xf000;
    else if (arg1 == 0x40020058)
        *0x40020004 |= 0xf0000;
}

void sub_192c(int32_t* arg1, int32_t arg2)
{
    if (!arg2)
        *arg1 &= 0xfffe;
    else
        *arg1 |= 1;
}

int32_t sub_1948(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int16_t* arg5)
{
    int32_t var_1c = arg3;
    int32_t var_20 = arg2;
    int32_t var_24 = arg1;
    *0x200000b4 = (arg5[1] * *arg5) << 1;
    *0x200000b8 = 0;
    *0x200000c0 = 0;
    sub_2db8(0x40010800, 0x100);
    sub_74e8(3);
    sub_74e8(*(arg5 + 4) << 8 >> 0x18);
    sub_74e8(arg5[2] << 0x10 >> 0x18);
    sub_74e8(arg5[2]);
    sub_2db8(0x40010800, 0x8000);
    sub_1808(arg1, arg2, arg3, arg4);
    sub_4978(0x2000018a, 0x200, 1);
    sub_31b4(0x2000018a, 0x200, 2);
    *0x20000058 = 0;
    int32_t i;
    
    do
        i = sub_4b1c(0x40014400, 0x80);
     while (i == 1);
    sub_4b08(0x40014400, 1, 0);
    sub_192c(0x40020030, 0);
    sub_4b5c(0x40014400);
    sub_4b08(0x40014400, 2, 1);
    sub_4b08(0x40014400, 1, 1);
    sub_4b08(0x40012000, 2, 1);
    
    if (*0x200000b4 - *0x200000b8 <= 0x200)
        *0x200000bc = *0x200000b4 - *0x200000b8;
    else
        *0x200000bc = 0x200;
    
    sub_192c(0x40020030, 0);
    sub_192c(0x40020044, 0);
    *0x4002003c = (*0x200000b0 << 9) + 0x2000018a;
    *0x40020034 = *0x200000bc;
    *0x40020048 = *0x200000bc;
    sub_1884(0x200, 0x40020000);
    sub_192c(0x40020030, 1);
    sub_192c(0x40020044, 1);
    sub_2dbc(0x40010c00, 0x80);
    
    while (true)
    {
        *0x20000058 = 0;
        int32_t i_1;
        
        do
            i_1 = sub_1c10(0x200, 0x40020000);
         while (!i_1);
        sub_192c(0x40020058, 0);
        *0x40020064 = (*0x200000b0 << 9) + 0x2000018a;
        *0x4002005c = *0x200000bc;
        sub_1884(0x20000, 0x40020000);
        sub_192c(0x40020058, 1);
        *0x200000b8 += *0x200000bc;
        
        if (*0x200000b8 >= *0x200000b4)
            break;
        
        if (*0x200000b4 - *0x200000b8 <= 0x200)
            *0x200000bc = *0x200000b4 - *0x200000b8;
        else
            *0x200000bc = 0x200;
        
        *0x200000b0 ^= 1;
        sub_192c(0x40020030, 0);
        sub_192c(0x40020044, 0);
        *0x4002003c = (*0x200000b0 << 9) + 0x2000018a;
        *0x40020034 = *0x200000bc;
        *0x40020048 = *0x200000bc;
        sub_1884(0x200, 0x40020000);
        sub_192c(0x40020030, 1);
        sub_192c(0x40020044, 1);
        *0x20000058 = 0;
        int32_t i_2;
        
        do
            i_2 = sub_1c10(0x20000, 0x40020000);
         while (!i_2);
    }
    
    *0x20000058 = 0;
    int32_t i_3;
    
    do
        i_3 = sub_1c10(0x20000, 0x40020000);
     while (!i_3);
    *0x20000058 = 0;
    int32_t i_4;
    
    do
        i_4 = sub_1c10(0x200, 0x40020000);
     while (!i_4);
    sub_52d0();
    sub_2dbc(0x40010800, 0x8000);
    *0x200000c0 = 1;
    return 1;
}

int32_t sub_1c10(int32_t arg1, int32_t* arg2)
{
    int32_t result;
    
    if (!(*arg2 & arg1))
        result = 0;
    else
        result = 1;
    
    if (*0x20000058 <= 0x50)
        return result;
    
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

int32_t sub_1c54(int32_t arg1, int32_t* arg2)
{
    if (!(*arg2 & arg1))
        return 0;
    
    return 1;
}

void sub_1c70(int32_t* arg1, int32_t* arg2)
{
    *arg1 = (*arg1 & 0xffff800f) | arg2[2] | arg2[8] | arg2[4] | arg2[5] | arg2[6] | arg2[7]
        | arg2[9] | arg2[0xa];
    arg1[1] = arg2[3];
    arg1[2] = *arg2;
    arg1[3] = arg2[1];
}

void sub_1cb0(int32_t arg1, int32_t, void* arg3, int32_t arg4)
{
    if (!arg4)
        *(arg3 + 0x10) = 0;
    else
        *(arg3 + 0x10) = arg1;
}

void sub_1cc0(int32_t arg1) __pure
{
    for (; arg1; arg1 -= 1)
    {
    }
}

void sub_1cca(int32_t arg1) __pure
{
    for (; arg1; arg1 -= 1)
    {
    }
}

int32_t sub_1cd4()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    int32_t var_18 = 0;
    int32_t var_10 = 0;
    int32_t var_14 = 2;
    var_1c = 8;
    sub_2ba0(0x40010800, &var_1c);
    int32_t var_24 = 8;
    char var_20 = 0;
    char var_1f = 0xc;
    char var_1e = 0;
    sub_1ef8(&var_24);
    sub_2b60(0, 3);
    char var_28 = 7;
    char var_27 = 1;
    char var_26 = 0;
    return sub_3c84(&var_28);
}

int32_t sub_1d30(int32_t arg1)
{
    int32_t r3;
    int32_t var_10_1 = r3;
    int32_t result = sub_6640(0x58, 0x30, *(0x38 * *0x20000150 + 0x8007ee4 + (arg1 << 3)), 
        *(0x38 * *0x20000150 + 0x8007ee4 + (arg1 << 3) + 2), 
        *(0x38 * *0x20000150 + 0x8007ee4 + (arg1 << 3) + 4));
    return result;
}

int32_t sub_1d7c()
{
    int32_t r3;
    int32_t var_8_1 = r3;
    return sub_6640(0xe, 0x7f, *(0x8007f54 + (*0x200000d8 << 3)), 
        *((*0x200000d8 << 3) + 0x8007f56), *((*0x200000d8 << 3) + 0x8007f58));
}

int32_t sub_1db0()
{
    int32_t r3;
    int32_t var_8_1 = r3;
    return sub_6640(5, 2, *(0x8007f64 + (*0x20000150 << 3)), *((*0x20000150 << 3) + 0x8007f66), 
        *((*0x20000150 << 3) + 0x8007f68));
}

int32_t sub_1de4(int32_t arg1)
{
    int32_t r3;
    int32_t var_10_1 = r3;
    int32_t result = sub_6640(0x58, 6, *(0x38 * *0x20000150 + 0x8007e74 + (arg1 << 3)), 
        *(0x38 * *0x20000150 + 0x8007e74 + (arg1 << 3) + 2), 
        *(0x38 * *0x20000150 + 0x8007e74 + (arg1 << 3) + 4));
    return result;
}

int32_t sub_1e30()
{
    if (sub_1ed0(1))
        sub_1ec4(1);
    
    int32_t result = sub_1ed0(2);
    
    if (result)
    {
        sub_1ec4(2);
        result = 2;
        *0x20000000 = 2;
    }
    
    return result;
}

int32_t sub_1e60()
{
    if (sub_1ed0(4))
        sub_1ec4(4);
    
    int32_t result = sub_1ed0(8);
    
    if (result)
    {
        sub_1ec4(8);
        
        if (!(*0x2000001c << 0x1f >> 0x1f))
            return sub_3810();
        
        result = 1;
        *0x20000000 = 1;
    }
    
    return result;
}

int32_t sub_1ea4()
{
    int32_t result = sub_1ed0(0x80);
    
    if (result)
    {
        sub_1ec4(0x80);
        result = 3;
        *0x20000000 = 3;
    }
    
    return result;
}

void sub_1ec4(int32_t arg1)
{
    *0x40010414 = arg1;
}

int32_t sub_1ed0(int32_t arg1)
{
    if (*0x40010414 & arg1 && *0x40010400 & arg1)
        return 1;
    
    return 0;
}

void sub_1ef8(int32_t* arg1)
{
    if (!*(arg1 + 6))
    {
        int32_t* r1_2 = arg1[1] + 0x40010400;
        *r1_2 &= ~*arg1;
        return;
    }
    
    *0x40010400 &= ~*arg1;
    *0x40010404 &= ~*arg1;
    int32_t* r1 = arg1[1] + 0x40010400;
    *r1 |= *arg1;
    *0x40010408 &= ~*arg1;
    *0x4001040c &= ~*arg1;
    
    if (*(arg1 + 5) != 0x10)
    {
        int32_t* r1_1 = *(arg1 + 5) + 0x40010400;
        *r1_1 |= *arg1;
    }
    else
    {
        *0x40010408 |= *arg1;
        *0x4001040c |= *arg1;
    }
}

int32_t sub_1f80()
{
    char var_28;
    int32_t var_24;
    int32_t var_1c;
    
    if (*0x20000006)
    {
        sub_2d88(&var_1c);
        int32_t var_18_2 = 0;
        int32_t var_10_2 = 0;
        int32_t var_14_2 = 2;
        var_1c = 8;
        sub_2ba0(0x40010800, &var_1c);
        int32_t var_18_3 = 0;
        int32_t var_10_3 = 0;
        int32_t var_14_3 = 1;
        var_1c = 2;
        sub_2ba0(0x40010c00, &var_1c);
        var_1c = 0x80;
        sub_2ba0(0x40010800, &var_1c);
        var_24 = 0x82;
        char var_20_2 = 0;
        char var_1f_2 = 0xc;
        char var_1e_2 = 1;
        sub_1ef8(&var_24);
        sub_2b60(1, 1);
        var_28 = 6;
        char var_27_3 = 1;
        char var_26_3 = 1;
        sub_3c84(&var_28);
        sub_2b60(0, 7);
        var_28 = 8;
        char var_27_4 = 1;
        char var_26_4 = 1;
        return sub_3c84(&var_28);
    }
    
    sub_2d88(&var_1c);
    int32_t var_18 = 0;
    int32_t var_10 = 0;
    int32_t var_14 = 2;
    var_1c = 8;
    sub_2ba0(0x40010800, &var_1c);
    int32_t var_18_1 = 0;
    int32_t var_10_1 = 0;
    int32_t var_14_1 = 1;
    var_1c = 2;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x80;
    sub_2ba0(0x40010800, &var_1c);
    var_24 = 8;
    char var_20 = 0;
    char var_1f = 8;
    char var_1e = 1;
    sub_1ef8(&var_24);
    var_24 = 0x82;
    char var_20_1 = 0;
    char var_1f_1 = 0xc;
    char var_1e_1 = 1;
    sub_1ef8(&var_24);
    sub_2b60(1, 1);
    var_28 = 6;
    char var_27 = 1;
    char var_26 = 1;
    sub_3c84(&var_28);
    sub_2b60(0, 3);
    var_28 = 7;
    char var_27_1 = 1;
    char var_26_1 = 1;
    sub_3c84(&var_28);
    sub_2b60(0, 7);
    var_28 = 8;
    char var_27_2 = 1;
    char var_26_2 = 1;
    return sub_3c84(&var_28);
}

int32_t sub_20dc(int32_t arg1)
{
    int32_t result = (*0x40022000 & 0x3c) | arg1;
    *0x40022000 = result;
    return result;
}

uint32_t sub_20f8()
{
    return *0x20000034;
}

int32_t sub_2104()
{
    *0x2000001c &= 0xbf;
    *0x2000005e = 0;
    *0x2000011a = 0;
    sub_59c6(0x40000400, *0x2000011a);
    return 0x40000400;
}

uint32_t sub_2138()
{
    return *0x20000030;
}

uint32_t sub_2144()
{
    return *0x20000032;
}

uint32_t sub_2150()
{
    uint32_t result = *0x20000019 << 0x1b >> 0x1f;
    
    if (!result)
    {
        if (*0x20000030 <= 0x3e8)
            result = *0x20000032;
        
        if (*0x20000030 > 0x3e8 || result > 0x3e8)
        {
            *0x2000005a += 1;
            result = *0x2000005a;
            
            if (result > 0x1e)
            {
                *0x2000005a = 0;
                sub_3d5c();
                /* no return */
            }
        }
    }
    
    return result;
}

uint32_t sub_219c()
{
    *0x20000160 += 1;
    uint32_t result = *0x20000160;
    
    if (result > 0x78)
    {
        *0x20000160 = 0;
        
        if (*0x20000162 >= 0x1e)
            *0x20000019 = (*0x20000019 & 0xfd) + 2;
        
        result = 0;
        *0x20000162 = 0;
    }
    
    return result;
}

uint32_t sub_21d8()
{
    *0x2000015e += 1;
    uint32_t result;
    
    if (*0x2000015e <= 0x7d0)
    {
        result = *0x20000164;
        
        if (result >= 0x32)
        {
            result = (*0x20000019 & 0xfffffffd) + 2;
            *0x20000019 = result;
        }
    }
    else
    {
        *0x2000015e = 0;
        
        if (*0x20000164 >= 0x32)
            *0x20000019 = (*0x20000019 & 0xfd) + 2;
        
        result = 0;
        *0x20000164 = 0;
    }
    
    return result;
}

int32_t sub_2234()
{
    uint32_t r0 = *0x200000f2;
    
    if (!r0)
    {
        sub_34e0(0xff);
        sub_3500(0xa5);
        sub_34e0(0x3e);
        sub_3500(8);
        sub_34e0(0x3a);
        sub_3500(0x65);
        sub_34e0(0x82);
        sub_3500(0);
        sub_34e0(0x98);
        sub_3500(0);
        sub_34e0(0x63);
        sub_3500(0xf);
        sub_34e0(0x64);
        sub_3500(0xf);
        sub_34e0(0xb4);
        sub_3500(0x34);
        sub_34e0(0xb5);
        sub_3500(0x30);
        sub_34e0(0x83);
        sub_3500(0x13);
        sub_34e0(0x86);
        sub_3500(4);
        sub_34e0(0x87);
        sub_3500(0x16);
        sub_34e0(0x88);
        sub_3500(0x25);
        sub_34e0(0x89);
        sub_3500(0x2f);
        sub_34e0(0x93);
        sub_3500(0x63);
        sub_34e0(0x96);
        sub_3500(0x81);
        sub_34e0(0xc3);
        sub_3500(0x10);
        sub_34e0(0xe6);
        sub_3500(0);
        sub_34e0(0x99);
        sub_3500(1);
        sub_34e0(0x44);
        sub_3500(0);
        sub_34e0(0x70);
        sub_3500(7);
        sub_34e0(0x71);
        sub_3500(0x19);
        sub_34e0(0x72);
        sub_3500(0x1a);
        sub_34e0(0x73);
        sub_3500(0x13);
        sub_34e0(0x74);
        sub_3500(0x19);
        sub_34e0(0x75);
        sub_3500(0x1d);
        sub_34e0(0x76);
        sub_3500(0x47);
        sub_34e0(0x77);
        sub_3500(0xa);
        sub_34e0(0x78);
        sub_3500(7);
        sub_34e0(0x79);
        sub_3500(0x47);
        sub_34e0(0x7a);
        sub_3500(5);
        sub_34e0(0x7b);
        sub_3500(9);
        sub_34e0(0x7c);
        sub_3500(0xd);
        sub_34e0(0x7d);
        sub_3500(0xc);
        sub_34e0(0x7e);
        sub_3500(0xc);
        sub_34e0(0x7f);
        sub_3500(8);
        sub_34e0(0xa0);
        sub_3500(0xb);
        sub_34e0(0xa1);
        sub_3500(0x36);
        sub_34e0(0xa2);
        sub_3500(9);
        sub_34e0(0xa3);
        sub_3500(0xd);
        sub_34e0(0xa4);
        sub_3500(8);
        sub_34e0(0xa5);
        sub_3500(0x23);
        sub_34e0(0xa6);
        sub_3500(0x3b);
        sub_34e0(0xa7);
        sub_3500(4);
        sub_34e0(0xa8);
        sub_3500(7);
        sub_34e0(0xa9);
        sub_3500(0x38);
        sub_34e0(0xaa);
        sub_3500(0xa);
        sub_34e0(0xab);
        sub_3500(0x12);
        sub_34e0(0xac);
        sub_3500(0xc);
        sub_34e0(0xad);
        sub_3500(7);
        sub_34e0(0xae);
        sub_3500(0x2f);
        sub_34e0(0xaf);
        sub_3500(7);
        sub_34e0(0xff);
        return sub_3500(0);
    }
    
    if (r0 != 1)
    {
        if (r0 != 2)
        {
            sub_3d78();
            /* no return */
        }
        
        sub_34e0(0xb1);
        sub_3500(5);
        sub_3500(0x3c);
        sub_3500(0x3c);
        sub_34e0(0xb2);
        sub_3500(5);
        sub_3500(0x3c);
        sub_3500(0x3c);
        sub_34e0(0xb3);
        sub_3500(5);
        sub_3500(0x3c);
        sub_3500(0x3c);
        sub_3500(5);
        sub_3500(0x3c);
        sub_3500(0x3c);
        sub_34e0(0xb4);
        sub_3500(3);
        sub_34e0(0xc0);
        sub_3500(0xa4);
        sub_3500(4);
        sub_3500(0x84);
        sub_34e0(0xc1);
        sub_3500(0xc1);
        sub_34e0(0xc2);
        sub_3500(0xd);
        sub_3500(0);
        sub_34e0(0xc3);
        sub_3500(0x8d);
        sub_3500(0x2a);
        sub_34e0(0xc4);
        sub_3500(0x8d);
        sub_3500(0xee);
        sub_34e0(0xc5);
        sub_3500(0xc);
        sub_34e0(0xe0);
        sub_3500(0xb);
        sub_3500(0x21);
        sub_3500(0xc);
        sub_3500(0x16);
        sub_3500(0x2e);
        sub_3500(0x27);
        sub_3500(0x1f);
        sub_3500(0x25);
        sub_3500(0x25);
        sub_3500(0x24);
        sub_3500(0x2c);
        sub_3500(0x38);
        sub_3500(0);
        sub_3500(3);
        sub_3500(0);
        sub_3500(0x10);
        sub_34e0(0xe1);
        sub_3500(0xe);
        sub_3500(0x1e);
        sub_3500(0xe);
        sub_3500(0x18);
        sub_3500(0x33);
        sub_3500(0x2d);
        sub_3500(0x28);
        sub_3500(0x2a);
        sub_3500(0x28);
        sub_3500(0x26);
        sub_3500(0x2f);
        sub_3500(0x3c);
        sub_3500(0);
        sub_3500(5);
        sub_3500(5);
        sub_3500(0x10);
        sub_34e0(0x35);
        sub_3500(0);
        sub_34e0(0x3a);
        return sub_3500(0x55);
    }
    
    sub_34e0(0xff);
    sub_3500(0xa5);
    sub_34e0(0x3e);
    sub_3500(8);
    sub_34e0(0x3a);
    sub_3500(0x65);
    sub_34e0(0x82);
    sub_3500(0);
    sub_34e0(0x98);
    sub_3500(0);
    sub_34e0(0x63);
    sub_3500(0xf);
    sub_34e0(0x64);
    sub_3500(0xf);
    sub_34e0(0xb4);
    sub_3500(0x34);
    sub_34e0(0xb5);
    sub_3500(0x30);
    sub_34e0(0x83);
    sub_3500(0x13);
    sub_34e0(0x86);
    sub_3500(4);
    sub_34e0(0x87);
    sub_3500(0x19);
    sub_34e0(0x88);
    sub_3500(0x2f);
    sub_34e0(0x89);
    sub_3500(0x36);
    sub_34e0(0x93);
    sub_3500(0x63);
    sub_34e0(0x96);
    sub_3500(0x81);
    sub_34e0(0xc3);
    sub_3500(0x10);
    sub_34e0(0xe6);
    sub_3500(0);
    sub_34e0(0x99);
    sub_3500(1);
    sub_34e0(0x44);
    sub_3500(0);
    sub_34e0(0x70);
    sub_3500(7);
    sub_34e0(0x71);
    sub_3500(0x19);
    sub_34e0(0x72);
    sub_3500(0x1a);
    sub_34e0(0x73);
    sub_3500(0x13);
    sub_34e0(0x74);
    sub_3500(0x19);
    sub_34e0(0x75);
    sub_3500(0x1d);
    sub_34e0(0x76);
    sub_3500(0x47);
    sub_34e0(0x77);
    sub_3500(0xa);
    sub_34e0(0x78);
    sub_3500(7);
    sub_34e0(0x79);
    sub_3500(0x47);
    sub_34e0(0x7a);
    sub_3500(5);
    sub_34e0(0x7b);
    sub_3500(9);
    sub_34e0(0x7c);
    sub_3500(0xd);
    sub_34e0(0x7d);
    sub_3500(0xc);
    sub_34e0(0x7e);
    sub_3500(0xc);
    sub_34e0(0x7f);
    sub_3500(8);
    sub_34e0(0xa0);
    sub_3500(0xb);
    sub_34e0(0xa1);
    sub_3500(0x36);
    sub_34e0(0xa2);
    sub_3500(9);
    sub_34e0(0xa3);
    sub_3500(0xd);
    sub_34e0(0xa4);
    sub_3500(8);
    sub_34e0(0xa5);
    sub_3500(0x23);
    sub_34e0(0xa6);
    sub_3500(0x3b);
    sub_34e0(0xa7);
    sub_3500(4);
    sub_34e0(0xa8);
    sub_3500(7);
    sub_34e0(0xa9);
    sub_3500(0x38);
    sub_34e0(0xaa);
    sub_3500(0xa);
    sub_34e0(0xab);
    sub_3500(0x12);
    sub_34e0(0xac);
    sub_3500(0xc);
    sub_34e0(0xad);
    sub_3500(7);
    sub_34e0(0xae);
    sub_3500(0x2f);
    sub_34e0(0xaf);
    sub_3500(7);
    sub_34e0(0xff);
    return sub_3500(0);
}

int32_t sub_290c()
{
    int32_t r3;
    int32_t var_8 = r3;
    sub_2dbc(0x40010c00, 0x40);
    sub_65e0(0xa);
    sub_2db8(0x40010c00, 0x40);
    sub_65e0(0xa);
    sub_2dbc(0x40010c00, 0x40);
    sub_65e0(0xa);
    sub_2db8(0x40010800, 0x8000);
    sub_34e0(0x11);
    sub_65e0(0x78);
    sub_2234();
    sub_34e0(0x36);
    uint32_t r0 = *0x200000f2;
    
    if (!r0)
        sub_3500(0x80);
    else if (r0 == 1)
        sub_3500(0x80);
    else
    {
        if (r0 != 2)
        {
            sub_3d78();
            /* no return */
        }
        
        sub_3500(0xc0);
    }
    
    sub_34e0(0x29);
    sub_65e0(0xa);
    sub_34e0(0x2c);
    sub_30a8(0, 0, 0x80, 0xa0, 0);
    *0x20000065 = 0xd;
    return 0xd;
}

int32_t sub_29c0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    uint32_t r0 = *0x20000065;
    int32_t r1;
    int32_t r2;
    int32_t lr;
    r1 = sub_5d76(r0, arg2, arg3, r0, arg4, lr);
    /* undefined */
}

int32_t sub_29dc()
{
    sub_2dbc(0x40010c00, 0x40);
    *0x20000065 = 1;
    *0x20000066 = 0;
    /* tailcall */
    return sub_2b3c(0x40010c00);
}

int32_t sub_29f2()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 > 5)
        *0x20000065 = 2;
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2a02()
{
    sub_2db8(0x40010c00, 0x40);
    *0x20000065 = 3;
    *0x20000066 = 0;
    /* tailcall */
    return sub_2b3c(0x40010c00);
}

int32_t sub_2a18()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 >= 0x14)
        *0x20000065 = 4;
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2a28()
{
    sub_2dbc(0x40010c00, 0x40);
    *0x20000065 = 5;
    /* tailcall */
    return sub_2b3c(0x40010c00);
}

int32_t sub_2a38()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 >= 5)
        *0x20000065 = 6;
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2a48()
{
    sub_2db8(0x40010800, 0x8000);
    int32_t r0 = sub_34e0(0x11);
    *0x20000065 = 7;
    *0x20000066 = 0;
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2a66()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 > 0x32)
        *0x20000065 = 9;
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2a76()
{
    sub_2234();
    sub_34e0(0x36);
    uint32_t r0 = *0x200000f2;
    
    if (!r0)
        sub_3500(0x80);
    else if (r0 == 1)
        sub_3500(0x80);
    else
    {
        if (r0 != 2)
        {
            sub_3d78();
            /* no return */
        }
        
        sub_3500(0xc0);
    }
    
    int32_t r0_1 = sub_34e0(0x29);
    *0x20000065 = 0xa;
    *0x20000066 = 0;
    /* tailcall */
    return sub_2b3c(r0_1);
}

int32_t sub_2ac4()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 > 0xa)
        *0x20000065 = 0xb;
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2ad4()
{
    int32_t r0 = sub_30a8(0, 0, 0x80, 0xa0, 0);
    *0x20000065 = 0xc;
    *0x20000066 = 0;
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t sub_2af0()
{
    uint32_t r0 = *0x20000066;
    
    if (r0 > 0xa)
    {
        if (*0x20000006 != 1)
        {
            if (*0x20000006 != 2)
                r0 = *0x20000006;
            
            if (*0x20000006 == 2 || r0 == 3)
                r0 = sub_3f84();
        }
        else
        {
            r0 = *0x20000000;
            
            if (r0 == 1)
            {
                sub_2f64();
                r0 = sub_3f84();
            }
        }
        
        *0x20000065 = 0xd;
        *0x20000066 = 0;
        *0x2000012a = 0;
    }
    
    /* tailcall */
    return sub_2b3c(r0);
}

int32_t j_sub_2b3c(int32_t arg1)
{
    /* tailcall */
    return sub_2b3c(arg1);
}

int32_t j_sub_2b3c(int32_t arg1)
{
    /* tailcall */
    return sub_2b3c(arg1);
}

int32_t sub_2b3c(int32_t arg1) __pure
{
    return;
}

void sub_2b60(int32_t arg1, int32_t arg2)
{
    *(0x40010008 + (arg2 >> 2 << 2)) &= ~(0xf << arg2 << 0x1e >> 0x1c);
    *(0x40010008 + (arg2 >> 2 << 2)) |= arg1 << arg2 << 0x1e >> 0x1c;
}


// set GPIO 

int32_t sub_2ba0(int32_t* arg1, int32_t* arg2)
{
    int32_t r1 = 0;
    int32_t result = 0;
    
    while (*arg2 >> r1)
    {
        int32_t r5_2 = *arg2 & 1 << r1;
        
        if (r5_2)
        {
            if (arg2[1] == 2 || arg2[1] == 0x12)
            {
                if (!(8 & r1))
                    arg1[8] =
                        (arg1[8] & ~(0xf << r1 << 0x1d >> 0x1b)) | arg2[5] << r1 << 0x1d >> 0x1b;
                else
                    arg1[9] =
                        (arg1[9] & ~(0xf << r1 << 0x1d >> 0x1b)) | arg2[5] << r1 << 0x1d >> 0x1b;
            }
            
            if (arg2[1] == 1 || arg2[1] == 0x11 || arg2[1] == 2 || arg2[1] == 0x12)
                arg1[1] = (arg1[1] & ~(1 << r1)) | arg2[1] << 0x1b >> 0x1f << r1;
            
            *arg1 = (*arg1 & ~(3 << r1 << 1)) | arg2[1] << 0x1e >> 0x1e << r1 << 1;
            arg1[3] = (arg1[3] & ~(3 << r1 << 1)) | arg2[2] << r1 << 1;
            arg1[2] = (arg1[2] & ~(1 << r1)) | arg2[3] << r1;
            result = (arg1[0xb] & ~(1 << r1)) | arg2[4] << r1;
            arg1[0xb] = result;
            
            if ((arg2[1] & 0x10000000) == 0x10000000)
            {
                int32_t r5_41;
                
                if (arg1 == 0x40010800)
                    r5_41 = 0;
                else if (arg1 == 0x40010c00)
                    r5_41 = 1;
                else if (arg1 != 0x40011000)
                    r5_41 = 5;
                else
                    r5_41 = 2;
                
                *(0x40010008 + (r1 >> 2 << 2)) = (*(0x40010008 + (r1 >> 2 << 2))
                    & ~(0xf << r1 << 0x1e >> 0x1c)) | r5_41 << r1 << 0x1e >> 0x1c;
                int32_t r0_24 = *0x40010400 & ~r5_2;
                
                if ((arg2[1] & __builtin_memcpy) == __builtin_memcpy)
                    r0_24 |= r5_2;
                
                *0x40010400 = r0_24;
                int32_t r0_26 = *0x40010404 & ~r5_2;
                
                if ((arg2[1] & 0x20000) == 0x20000)
                    r0_26 |= r5_2;
                
                *0x40010404 = r0_26;
                int32_t r0_28 = *0x40010408 & ~r5_2;
                
                if ((arg2[1] & 0x100000) == 0x100000)
                    r0_28 |= r5_2;
                
                *0x40010408 = r0_28;
                result = *0x4001040c & ~r5_2;
                
                if ((arg2[1] & 0x200000) == 0x200000)
                    result |= r5_2;
                
                *0x4001040c = result;
            }
        }
        
        r1 += 1;
    }
    
    return result;
}

void sub_2d88(int32_t* arg1)
{
    *arg1 = 0xffff;
    arg1[5] = 0xf;
    arg1[1] = 0;
    arg1[2] = 0;
    arg1[3] = 1;
    arg1[4] = 0;
}

int32_t sub_2da4(void* arg1, int32_t arg2)
{
    if (!(*(arg1 + 0x10) & arg2))
        return 0;
    
    return 1;
}

void sub_2db8(void* arg1, int32_t arg2)
{
    *(arg1 + 0x28) = arg2;
}

void sub_2dbc(void* arg1, int32_t arg2)
{
    *(arg1 + 0x18) = arg2;
}

int32_t sub_2dc0()
{
    if ((*0x40012008 & 0x80) != 0x80)
        return 0;
    
    return 1;
}

int32_t sub_2dd8()
{
    if ((*0x40012008 & 2) != 2)
        return 1;
    
    return 0;
}

int32_t sub_2df0()
{
    if ((*0x40014408 & 0x80) != 0x80)
        return 0;
    
    return 1;
}

uint32_t sub_2e08()
{
    uint32_t result = *0x2000005e;
    
    if (result)
        return 1;
    
    return result;
}

int32_t sub_2e1c()
{
    if (*0x20000026 > 0x29)
        *0x20000026 = 0;
    
    char result = *(0x8009370 + *0x20000026);
    *0x2000011e = result;
    return result;
}

int32_t sub_2e44()
{
    if (*0x20000065 != 0xd)
        return 0;
    
    return 1;
}

int32_t sub_2e58()
{
    char result;
    
    if (*0x20000120 >= 1)
    {
        result = *0x20000019 & 0xf7;
        *0x20000019 = result;
    }
    else
    {
        result = (*0x20000019 & 0xf7) + 8;
        *0x20000019 = result;
    }
    
    return result;
}

void sub_2e8a() __noreturn
{
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

void sub_2ea8(int32_t arg1)
{
    *0x40003008 = arg1;
}

int32_t sub_2eb4()
{
    *0x40003000 = 0xcccc;
    return 0xcccc;
}

int32_t sub_2ec4()
{
    *0x40003000 = 0xaaaa;
    return 0xaaaa;
}

void sub_2ed4(int32_t arg1)
{
    *0x40003004 = arg1;
}

void sub_2ee0(int32_t arg1)
{
    *0x40003000 = arg1;
}

int32_t sub_2eec()
{
    char result = *0x20000027 + 1;
    *0x20000027 = result;
    return result;
}

int32_t sub_2efc()
{
    int16_t result = *0x2000002a + 1;
    *0x2000002a = result;
    return result;
}

int32_t sub_2f0c()
{
    sub_3520(1, sub_46fc(1), 0x40021000);
    sub_450c(0x600000);
    sub_467c(0xc, 1);
    sub_359c(0x40000c00, 0);
    sub_3576(0x40000c00);
    sub_356c(0x40000c00);
    sub_3590(0x40000c00, 0xfde8);
    sub_3596(0x40000c00, 0xea60);
    sub_35aa(0x40000c00, 4);
    return 0x40000c00;
}

int32_t sub_2f64()
{
    int32_t r3;
    int32_t var_10_2 = r3;
    
    if (*0x20000150)
        sub_6640(0, 0, *0x8007f7c, *0x8007f7e, *0x8007f80);
    else
        sub_6640(0, 0, *0x8007f74, *0x8007f76, *0x8007f78);
    
    sub_1de4(*0x20000120);
    sub_1d30(sub_15a0());
    sub_1db0();
    
    if (*0x20000150)
        sub_6640(5, 0x56, 0x76, 0x20, 0x26a60);
    else
        sub_6640(5, 0x56, 0x76, 0x20, 0x26a60);
    
    sub_1d7c();
    *0x20000026 = 1;
    *0x20000027 = 0;
    return 0;
}

uint32_t sub_3008(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    uint32_t result = *0x20000065;
    
    if (result != 0xd)
        return result;
    
    *0x2000012a = 0;
    *0x20000006 = 0;
    sub_2f64();
    return sub_3f84();
}

int32_t sub_3038()
{
    sub_463c(0x1001, 1);
    sub_940(0, 4);
    return sub_43e8(0, 0x3800);
}

int32_t sub_305c()
{
    int32_t r3;
    int32_t var_18_1 = r3;
    int32_t r0_1;
    int32_t r1;
    r0_1 = sub_618(*0x20000130);
    int32_t r0_2;
    int32_t r1_1;
    r0_2 = sub_528(r0_1, r1, 0, 0x40f86a00);
    return sub_710(r0_2, r1_1);
}

int32_t sub_3084()
{
    sub_2ee0(&data_5555);
    sub_2ed4(5);
    sub_2ea8(0x398);
    sub_2ec4();
    sub_2eb4();
    return 0xcccc;
}

int32_t sub_30a8(char arg1, char arg2, int32_t arg3, int32_t arg4, int16_t arg5)
{
    int32_t var_18 = arg4;
    int32_t var_1c = arg3;
    uint32_t i = 0;
    int32_t var_28 = 0;
    sub_5bcc();
    *0x20000080 = arg5;
    *0x20000081 = *arg5[1];
    *0x20000082 = arg5;
    *0x20000083 = *arg5[1];
    *0x2000007c = 0x20000080;
    int32_t r7_1 = (arg4 * arg3) << 1;
    *0x20000070 = sub_f4(r7_1, 0xffff);
    sub_2db8(0x40010800, 0x8000);
    sub_1808(arg1, arg2, arg3, arg4);
    sub_2dbc(0x40010c00, 0x80);
    sub_3460(1);
    
    if (!*0x20000070)
        sub_31b4(*0x2000007c, r7_1, 1);
    else
    {
        *0x20000074 = 0xffff;
        *0x20000078 = sub_f4(r7_1, *0x20000074);
        
        for (; i < *0x20000070; i = i + 1)
        {
            sub_31b4(*0x2000007c, *0x20000074, 1);
            sub_5bf8();
        }
        
        if (*0x20000070)
            sub_31b4(*0x2000007c, *0x20000078, 1);
    }
    
    sub_5bf8();
    sub_2dbc(0x40010800, 0x8000);
    return sub_3460(0);
}

int32_t sub_31b4(int32_t arg1, int32_t arg2, int32_t arg3)
{
    sub_188c(0x40020058);
    sub_2db8(0x40010800, 0x8000);
    int32_t var_3c = 0x4001200c;
    int32_t var_38 = arg1;
    int32_t var_34 = 0x10;
    int32_t var_30 = arg2;
    int32_t var_2c = 0;
    int32_t var_28 = 0x80;
    int32_t var_1c = 0;
    int32_t var_14 = 0;
    int32_t var_18 = 0x2000;
    
    if (arg3 == 2)
    {
        int32_t var_24_1 = 0x200;
        int32_t var_20_1 = 0;
        sub_1c70(0x40020058, &var_3c);
        return sub_1cb0(0xd, 0x40020000, 0x40020058, 1);
    }
    
    int32_t var_28_1 = 0;
    
    if (!arg3)
    {
        int32_t var_24_2 = 0x200;
        int32_t var_20_2 = 0;
    }
    else if (arg3 == 1)
    {
        int32_t r0_1 = arg3 << 8;
        int32_t var_24_3 = r0_1;
        int32_t var_20_3 = r0_1 << 2;
    }
    else if (arg3 == 3)
    {
        int32_t var_28_2 = 0x80;
        int32_t var_24_4 = 0x200;
        int32_t var_20_4 = 0;
    }
    
    sub_1c70(0x40020058, &var_3c);
    sub_1cb0(0xd, 0x40020000, 0x40020058, 1);
    sub_4b08(0x40012000, 2, 1);
    sub_192c(0x40020058, 1);
    return 0x40020058;
}

int32_t sub_3280()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    sub_46bc(0xd, 1);
    sub_2db8(0x40010c00, 8);
    var_1c = 8;
    int32_t var_10 = 0;
    int32_t var_18 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 0x20);
    var_1c = 0x20;
    int32_t var_10_1 = 0;
    int32_t var_18_1 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010800, 0x8000);
    var_1c = 0x8000;
    int32_t var_10_2 = 0;
    int32_t var_18_2 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010c00, 0x80);
    var_1c = 0x80;
    int32_t var_10_3 = 0;
    int32_t var_18_3 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2dbc(0x40010c00, 0x40);
    var_1c = 0x40;
    int32_t var_10_4 = 0;
    int32_t var_18_4 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2dbc(0x40010c00, 0x40);
    sub_65e0(0xa);
    sub_2db8(0x40010c00, 0x40);
    sub_65e0(0x64);
    sub_2dbc(0x40010c00, 0x40);
    sub_65e0(0x78);
    return 0x78;
}

int32_t sub_3358()
{
    sub_46bc(0xc, 1);
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 0xc0;
    int32_t var_18 = 1;
    return sub_2ba0(0x40010c00, &var_1c);
}

int32_t sub_3384()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    sub_46bc(0xc, 1);
    sub_46bc(0x201, 1);
    sub_463c(1, 1);
    var_1c = 8;
    int32_t var_18 = 2;
    int32_t var_8 = 0;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x20;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x8000;
    int32_t var_18_1 = 1;
    return sub_2ba0(0x40010800, &var_1c);
}


// SPI1 initialization?
// for flash ic?

int32_t sub_33e8()
{
    sub_4a5c(0x40012000, 0);
    sub_4acc(0x40012000);
    sub_46bc(0x200, 0);
    int32_t var_1c = 8;
    int32_t var_18 = 0;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x20;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x8000;
    return sub_2ba0(0x40010800, &var_1c);
}

int32_t sub_3440()
{
    sub_3384();
    sub_3358();
    sub_3460(0);
    sub_4a5c(0x40012000, 1);
    return 0x40012000;
}


// Set SPI 1 register It sets the Master Mode (MSTR) bit and configures the Baud Rate Prescaler (BR) to determine the SPI speed.


int32_t sub_3460(int32_t arg1)
{
    sub_2dbc(0x40010800, 0x8000);
    
    if (arg1)
        int16_t var_18_1 = 0x800;
    else
        int16_t var_18 = 0;
    
    int16_t var_1c = 0;
    int16_t var_1a = 0x104;
    int16_t var_16 = 2;
    int16_t var_14 = 1;
    int16_t var_12 = 0x200;
    int16_t var_10 = 8;
    int16_t var_e = 0;
    int16_t var_c = 7;
    return sub_4b68(0x40012000, &var_1c);
}

int32_t sub_34bc(int32_t arg1, int16_t arg2)
{
    sub_2dbc(0x40010c00, 0x80);
    sub_31b4(arg1, arg2, 3);
    return sub_5bf8();
}

int32_t sub_34e0(int32_t arg1)
{
    int32_t var_8 = arg1;
    sub_2db8(0x40010c00, 0x80);
    sub_31b4(&var_8, 1, 0);
    return sub_5bf8();
}

int32_t sub_3500(int32_t arg1)
{
    int32_t var_8 = arg1;
    sub_2dbc(0x40010c00, 0x80);
    sub_31b4(&var_8, 1, 0);
    return sub_5bf8();
}

int32_t sub_3520(char arg1, int32_t arg2, int32_t arg3)
{
    int32_t r3;
    int32_t var_10 = r3;
    int32_t var_14 = arg3;
    int32_t var_18 = arg2;
    sub_1ec4(0x800000);
    var_14 = 0x800000;
    var_10 = 0;
    *var_10[1] = 8;
    *var_10[2] = 1;
    sub_1ef8(&var_14);
    var_18 = 0x14;
    *var_18[1] = 1;
    *var_18[2] = arg1;
    return sub_3c84(&var_18);
}

void sub_3558(void* arg1)
{
    *(arg1 + 4) |= 1;
}

void sub_3562(void* arg1)
{
    *(arg1 + 0x10) = *(arg1 + 0x10) >> 1 << 1;
}

void sub_356c(void* arg1)
{
    *(arg1 + 0x10) |= 1;
}

void sub_3576(void* arg1)
{
    *(arg1 + 8) |= 1;
}

int32_t sub_3580(int16_t* arg1)
{
    return *arg1 << 0x1f >> 0x1f;
}

void sub_3590(void* arg1, int16_t arg2)
{
    *(arg1 + 0x18);
    *(arg1 + 0x18) = arg2;
}

void sub_3596(void* arg1, int16_t arg2)
{
    *(arg1 + 0x14);
    *(arg1 + 0x14) = arg2;
}

void sub_359c(void* arg1, int32_t arg2)
{
    *(arg1 + 0xc) = (*(arg1 + 0xc) & 0xfffff1ff) | arg2;
}

void sub_35aa(void* arg1, int16_t arg2)
{
    *(arg1 + 0x10) = (*(arg1 + 0x10) & 0xfff9) | arg2;
}

int32_t sub_35b8()
{
    if (sub_572e(0x40001000, 1))
    {
        sub_5660(0x40001000, 1);
        sub_1544();
        uint32_t r0_3 = *0x20000055 + 1;
        *0x20000055 = r0_3;
        
        if (r0_3 > 3)
        {
            *0x20000055 = 0;
            sub_77c4();
            sub_7a90(1);
            sub_62b4(1);
            sub_7768(1);
        }
        
        uint32_t r0_6 = *0x20000054 + 1;
        *0x20000054 = r0_6;
        
        if (r0_6 > 1)
        {
            *0x20000054 = 0;
            *0x2000004a += 1;
            sub_6628();
            sub_6d38();
            sub_7c94();
            sub_7780(1);
            sub_219c();
            sub_3b1c();
            sub_4d24(1);
            sub_2150();
            sub_2efc();
            sub_7774(1);
            
            if (*0x20000058 < 0xfffa)
                *0x20000058 += 1;
            
            uint32_t r0_14 = *0x20000056 + 1;
            *0x20000056 = r0_14;
            
            if (r0_14 > 9)
            {
                *0x20000056 = 0;
                *0x2000004c += 1;
                
                if (*0x2000012a < 0xfffa)
                    *0x2000012a += 1;
                
                if (*0x20000019 << 0x1a >> 0x1f)
                {
                    if (*0x20000124 < 0xfffa)
                        *0x20000124 += 1;
                    
                    if (*0x20000124 > 0x3e8)
                    {
                        *0x20000128 = 0x3e8;
                        
                        if (*0x20000150)
                            *0x20000144 += 0x686;
                        else
                        {
                            *0x20000144 += 0x3e8;
                            *0x20000158 += 0x3e8;
                        }
                        
                        *0x20000124 = 0;
                        *0x2000012a = 0;
                        *0x20000006 = 0xb;
                        sub_72e8();
                    }
                }
                
                sub_547c();
                sub_21d8();
                *0x2000001a = (*0x2000001a & 0xfb) + 4;
                sub_1348();
                sub_3e78();
                sub_2eec();
                sub_6958();
            }
        }
    }
    
    int32_t result = sub_3580(0x40000c00);
    
    if (result)
    {
        *0x20000000 = 4;
        sub_3558(0x40000c00);
        result = 0x800000;
        sub_1ec4(0x800000);
    }
    
    return result;
}

int32_t sub_3790()
{
    int32_t r3;
    int32_t var_8 = r3;
    return sub_30a8(0, 0, 0x80, 0xa0, 0);
}

uint32_t sub_37a4()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    uint32_t result = *0x2000001b << 0x19 >> 0x1f;
    
    if (!result)
        return result;
    
    *0x2000001b &= 0xbf;
    return sub_4870();
}

int32_t sub_3810()
{
    int32_t result = *0x40010810 & 8;
    
    if (result == 1)
    {
        result = *0x40010810 & 8;
        
        if (result == 1)
        {
            result = *0x20000162 + 1;
            *0x20000162 = result;
        }
    }
    
    return result;
}

int32_t sub_3844()
{
    int32_t var_1c = 0;
    int32_t var_58;
    __builtin_memset(&var_58, 0, 0x14);
    sub_4d38(0xa65);
    int16_t var_24 = *0x20000034;
    int16_t var_2c = *0x20000030;
    int16_t var_34 = *0x2000012c;
    int16_t var_22 = *0x20000034;
    int16_t var_2a = *0x20000030;
    int16_t var_32 = *0x2000012c;
    int16_t var_20 = *0x20000034;
    int16_t var_28 = *0x20000030;
    int16_t var_30 = *0x2000012c;
    int16_t var_1e = *0x20000034;
    int16_t var_26 = *0x20000030;
    int16_t var_2e = *0x2000012c;
    uint32_t r0_5 = sub_5db0(&var_24, 4);
    uint32_t r0_7 = sub_5db0(&var_2c, 4);
    uint32_t r0_9 = sub_5db0(&var_34, 4);
    sub_65a8();
    int32_t r0_17;
    int32_t r1_12;
    r0_17 = sub_618(r0_9 * (r0_5 << 1));
    int32_t r0_18;
    int32_t r1_13;
    r0_18 = sub_528(r0_17, r1_12, 0, 0x40f86a00);
    int32_t var_5c = r1_13;
    int32_t var_60 = r0_18;
    int32_t r0_19 = sub_710(r0_18, r1_13);
    int32_t r0_21;
    int32_t r1_14;
    r0_21 = sub_618(r0_9 * (r0_7 << 1));
    int32_t r0_22;
    int32_t r1_15;
    r0_22 = sub_528(r0_21, r1_14, 0, 0x40f86a00);
    int32_t var_5c_1 = r1_15;
    int32_t var_60_1 = r0_22;
    int32_t r0_23 = sub_710(r0_22, r1_15);
    
    if (TEST_BIT(r0_7, 0x1f))
        return 0x411e6666;
    
    uint32_t r0_28;
    int32_t r1_18;
    r0_28 = sub_698(sub_1ea(r0_19, r0_23));
    int32_t r0_29;
    int32_t r1_19;
    r0_29 = sub_528(r0_28, r1_18, 0x9999999a, 0x3f999999);
    int32_t var_5c_2 = r1_19;
    int32_t var_60_2 = r0_29;
    return sub_274(r0_23, sub_710(r0_29, r1_19));
}

int32_t sub_39b0()
{
    int32_t var_1c = 0;
    int32_t var_58;
    __builtin_memset(&var_58, 0, 0x14);
    sub_4d54(0xa65);
    int16_t var_24 = *0x20000034;
    int16_t var_2c = *0x20000032;
    int16_t var_34 = *0x2000012c;
    int16_t var_22 = *0x20000034;
    int16_t var_2a = *0x20000032;
    int16_t var_32 = *0x2000012c;
    int16_t var_20 = *0x20000034;
    int16_t var_28 = *0x20000032;
    int16_t var_30 = *0x2000012c;
    int16_t var_1e = *0x20000034;
    int16_t var_26 = *0x20000032;
    int16_t var_2e = *0x2000012c;
    uint32_t r0_5 = sub_5db0(&var_24, 4);
    uint32_t r0_7 = sub_5db0(&var_2c, 4);
    uint32_t r0_9 = sub_5db0(&var_34, 4);
    sub_65c4();
    int32_t r0_17;
    int32_t r1_12;
    r0_17 = sub_618(r0_9 * (r0_5 << 1));
    int32_t r0_18;
    int32_t r1_13;
    r0_18 = sub_528(r0_17, r1_12, 0, 0x40f86a00);
    int32_t var_5c = r1_13;
    int32_t var_60 = r0_18;
    int32_t r0_19 = sub_710(r0_18, r1_13);
    int32_t r0_21;
    int32_t r1_14;
    r0_21 = sub_618(r0_9 * (r0_7 << 1));
    int32_t r0_22;
    int32_t r1_15;
    r0_22 = sub_528(r0_21, r1_14, 0, 0x40f86a00);
    int32_t var_5c_1 = r1_15;
    int32_t var_60_1 = r0_22;
    int32_t r0_23 = sub_710(r0_22, r1_15);
    
    if (TEST_BIT(r0_7, 0x1f))
        return 0x411e6666;
    
    uint32_t r0_28;
    int32_t r1_18;
    r0_28 = sub_698(sub_1ea(r0_19, r0_23));
    int32_t r0_29;
    int32_t r1_19;
    r0_29 = sub_528(r0_28, r1_18, 0x9999999a, 0x3f999999);
    int32_t var_5c_2 = r1_19;
    int32_t var_60_2 = r0_29;
    return sub_274(r0_23, sub_710(r0_29, r1_19));
}

uint32_t sub_3b1c()
{
    uint32_t result;
    
    if (*0x40010810 & 8)
    {
        result = *0x2000015a;
        
        if (result < 0xfffa)
        {
            result = *0x2000015a + 1;
            *0x2000015a = result;
        }
    }
    else
    {
        result = *0x2000015c;
        
        if (result < 0xfffa)
        {
            result = *0x2000015c + 1;
            *0x2000015c = result;
        }
    }
    
    return result;
}

uint32_t sub_3b60()
{
    int32_t r4 = 0;
    uint32_t result = *0x40010810 & 8;
    
    if (result)
        r4 = 1;
    
    if (r4)
    {
        *0x2000015c = 0;
        
        if (*0x2000015a <= 0x4e20)
        {
            result = *0x2000015a;
            
            if (result > 1)
            {
                *0x2000012a = 0;
                result = *0x20000019 << 0x1e >> 0x1f;
                
                if (!result)
                {
                    result = *0x2000001a << 0x1b >> 0x1f;
                    
                    if (!result)
                    {
                        *0x2000001a = (*0x2000001a & 0xef) + 0x10;
                        *0x20000164 += 1;
                        sub_5074();
                        result = *0x20000019 << 0x1a >> 0x1f;
                        
                        if (result)
                        {
                            if (*0x20000006 != 1)
                            {
                                result = *0x20000006;
                                
                                if (result != 2)
                                {
                                    *0x20000006 = 3;
                                    return sub_3f84();
                                }
                            }
                            else
                            {
                                result = 2;
                                *0x20000006 = 2;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            result = (*0x20000019 & 0xfffffffd) + 2;
            *0x20000019 = result;
        }
    }
    else if (!r4)
    {
        result = *0x2000015c;
        
        if (result > 1)
        {
            *0x2000015c = 0;
            *0x2000015a = 0;
            *0x20000019 &= 0xef;
            *0x2000001b &= 0xfb;
            *0x2000001a &= 0xef;
            result = *0x20000019 << 0x1a >> 0x1f;
            
            if (result)
            {
                *0x20000019 &= 0xdf;
                return sub_1518();
            }
        }
    }
    
    return result;
}

int32_t sub_3c80() __pure
{
    return;
}

int32_t sub_3c84(void* arg1)
{
    sub_3cc8(*arg1, *(arg1 + 1));
    int32_t result;
    
    if (!*(arg1 + 2))
    {
        result = *arg1;
        *0xe000e180 = 1 << result << 0x1b >> 0x1b;
    }
    else
    {
        result = *arg1;
        *0xe000e100 = 1 << result << 0x1b >> 0x1b;
    }
    
    return result;
}

void sub_3cc8(int32_t arg1, int32_t arg2)
{
    if (arg1 >= 0)
        *(0xe000e400 + (arg1 >> 2 << 2)) = (*(0xe000e400 + (arg1 >> 2 << 2))
            & ~(0xff << arg1 << 0x1e >> 0x1b)) | arg2 << 0x1e >> 0x18 << arg1 << 0x1e >> 0x1b;
    else
    {
        int32_t r2_2 = (*(0xe000ed1c + (((arg1 << 0x1c >> 0x1c) - 8) >> 2 << 2))
            & ~(0xff << arg1 << 0x1e >> 0x1b)) | arg2 << 0x1e >> 0x18 << arg1 << 0x1e >> 0x1b;
        *(0xe000ed1c + (((arg1 << 0x1c >> 0x1c) - 8) >> 2 << 2)) = r2_2;
    }
}

void sub_3d40() __noreturn
{
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

void sub_3d5c() __noreturn
{
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

void sub_3d78() __noreturn
{
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

uint32_t sub_3d94()
{
    int32_t r3;
    int32_t var_8 = r3;
    uint32_t result;
    
    if (*0x20000150)
    {
        result = *0x20000027;
        
        if (result > 8)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 > 0x1f)
            {
                *0x20000026 = 0;
                return sub_6640(5, 0x56, 0x76, 0x20, 0x1d80 * *0x20000026 + 0x61a60);
            }
            
            sub_6640(5, 0x56, 0x76, 0x20, 0x1d80 * *0x20000026 + 0x61a60);
            result = *0x20000026 + 1;
            *0x20000026 = result;
        }
    }
    else
    {
        result = *0x20000027;
        
        if (result > 8)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 > 0x1f)
            {
                *0x20000026 = 0;
                return sub_6640(5, 0x56, 0x76, 0x20, 0x1d80 * *0x20000026 + 0x26a60);
            }
            
            sub_6640(5, 0x56, 0x76, 0x20, 0x1d80 * *0x20000026 + 0x26a60);
            result = *0x20000026 + 1;
            *0x20000026 = result;
        }
    }
    
    return result;
}

uint32_t sub_3e78()
{
    if (!*0x200000c9)
    {
        if (*0x40010c10 & 2)
        {
            *0x20000020 += 1;
            
            if (*0x20000020 > 2)
            {
                *0x2000001e = 0;
                *0x2000001a = (*0x2000001a & 0xbf) + 0x40;
                *0x2000001b = *0x2000001b >> 1 << 1;
            }
        }
        else
        {
            *0x2000001e += 1;
            
            if (*0x2000001e > 2)
            {
                *0x20000020 = 0;
                *0x2000001b = (*0x2000001b >> 1 << 1) + 1;
                
                if (*0x2000001b << 0x1f >> 0x1f && *0x2000001a << 0x19 >> 0x1f)
                {
                    *0x20000022 += 1;
                    *0x2000001a &= 0xbf;
                    *0x20000019 &= 0xfd;
                }
            }
        }
    }
    
    uint32_t result = *0x20000022;
    
    if (result > 0)
    {
        *0x20000024 += 1;
        result = *0x20000024;
        
        if (result > 0x3e8)
        {
            if (*0x20000022 >= 5)
                *0x2000001a = (*0x2000001a & 0xdf) + 0x20;
            
            result = 0;
            *0x20000024 = 0;
            *0x20000022 = 0;
        }
    }
    
    return result;
}

uint32_t sub_3f84()
{
    uint32_t result;
    
    if (*0x200000d8)
    {
        result = *0x200000d8;
        
        if (result == 1)
        {
            result = *0x2000001b << 0x19 >> 0x1f;
            
            if (!result)
            {
                *0x2000005e = 0x64;
                *0x2000005f = *0x2000005e;
                *0x2000011a = 0xfa1;
                sub_59c6(0x40000400, *0x2000011a);
                result = (*0x2000001c & 0xffffffbf) + 0x40;
                *0x2000001c = result;
            }
        }
    }
    else
    {
        result = *0x2000001b << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000005e = 0x3c;
            *0x2000005f = *0x2000005e;
            *0x2000011a = 0x4b0;
            sub_59c6(0x40000400, *0x2000011a);
            result = (*0x2000001c & 0xffffffbf) + 0x40;
            *0x2000001c = result;
        }
    }
    
    return result;
}

uint32_t sub_402c()
{
    uint32_t result = *0x20000068;
    
    if (!result)
    {
        sub_63cc();
        result = 1;
        *0x20000068 = 1;
    }
    
    return result;
}

uint32_t sub_4048()
{
    uint32_t result;
    
    if (*0x200000ec < 5)
    {
        result = 0;
        *0x200000ec = 0;
    }
    else
    {
        *0x200000ec = 0;
        
        if (*0x20000006 && *0x20000006 != 9 && *0x20000006 != 8 && *0x20000006 != 3
                && *0x20000006 != 0xa)
            result = *0x20000006;
        
        if (!*0x20000006 || *0x20000006 == 9 || *0x20000006 == 8 || *0x20000006 == 3
            || *0x20000006 == 0xa || result == 0xe)
        {
            *0x20000006 = 5;
            return sub_72e8();
        }
    }
    
    return result;
}

int32_t sub_40a4()
{
    sub_46bc(0x1000, 1);
    sub_46bc(0x2000, 1);
    sub_467c(2, 1);
    return 2;
}

int32_t sub_40c0()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 0x20;
    int32_t var_18 = 2;
    int32_t var_c = 1;
    int32_t var_8 = 3;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x100;
    int32_t var_18_1 = 2;
    int32_t var_c_1 = 1;
    int32_t var_8_1 = 5;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x40;
    int32_t var_18_2 = 2;
    int32_t var_c_2 = 1;
    int32_t var_8_2 = 2;
    return sub_2ba0(0x40010800, &var_1c);
}

int32_t sub_4120()
{
    int32_t var_1c = 0x40;
    int32_t var_10 = 0;
    int32_t var_18 = 0x11;
    sub_2ba0(0x40010800, &var_1c);
    sub_2dbc(0x40010800, 0x40);
    var_1c = 0x20;
    int32_t var_10_1 = 0;
    int32_t var_18_1 = 0x11;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x20);
    var_1c = 0x100;
    int32_t var_10_2 = 0;
    int32_t var_18_2 = 0x11;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 0x100);
    return 0x40010c00;
}

// Sleep Mode Preparation Disabling non-essential peripherals
int32_t sub_4188()
{
    sub_40a4();
    sub_5520();
    sub_54ac();
    sub_55d0();
    return sub_40c0();
}


// Sleep Mode Function

void sub_41a0(int32_t arg1)
{
    *0x40007000 |= *0x40007000 >> 2 << 2;
    *0xe000ed10 |= 4;
    
    if (arg1 != 1)
    {
        __sev();
        __wfe();
        __wfe();
    }
    else
        __wfi();
    
    *0xe000ed10 &= 0xfffffffb;
}

int32_t sub_41e4(int32_t arg1, int32_t arg2, int32_t arg3 @ r5)
{
    int32_t var_10 = arg3;
    int32_t var_18 = arg2;
    sub_45e0();
    
    if (!arg2)
    {
        sub_44e4(1);
        
        if (sub_47d8() != 1)
        {
            sub_3d40();
            /* no return */
        }
    }
    else if (arg2 == 1)
    {
        sub_448c(arg2 << 0x10);
        
        if (sub_479c() != 1)
        {
            sub_3d40();
            /* no return */
        }
    }
    
    int32_t var_24;
    
    if (arg1 == 0x1e84800)
    {
        var_24 = 1;
        
        if (!arg2)
            arg3 = 0x50000;
        else if (arg2 == 1)
            arg3 = 0x50000;
        
        int32_t var_2c = 0x400;
        int32_t var_30 = 0;
    }
    else
    {
        if (arg1 != 0x2dc6c00)
        {
            sub_3d40();
            /* no return */
        }
        
        var_24 = 2;
        
        if (!arg2)
            arg3 = 0x90000;
        else if (arg2 == 1)
            arg3 = 0x90000;
        
        int32_t var_2c_1 = 0x400;
        int32_t var_30_1 = 0;
    }
    
    sub_20dc(var_24);
    sub_446c(0);
    sub_454c(0);
    sub_4528(0x400);
    
    if (arg2 != 1)
        sub_456c(0, arg3, 0x100000, 0);
    else
    {
        int32_t r2_1 = arg2 << 0x14;
        sub_456c(r2_1 << 4, arg3, r2_1, 0);
    }
    
    sub_4720(1);
    int32_t var_28 = 0x1e8480;
    
    while (!(*0x40021000 & 0x2000000))
    {
        int32_t r0_8 = var_28;
        var_28 = r0_8 - 1;
        
        if (!r0_8)
        {
            sub_3d40();
            /* no return */
        }
    }
    
    sub_45c4(2);
    int32_t i_1 = 0x1e8480;
    int32_t i;
    
    do
    {
        int32_t result = sub_4788();
        
        if (result == 0x4000)
            return result;
        
        i = i_1;
        i_1 = i - 1;
    } while (i);
    sub_3d40();
    /* no return */
}

int32_t sub_4324() __pure
{
    return;
}

uint32_t sub_4328()
{
    uint32_t result = *0x2000001a << 0x1d >> 0x1f;
    
    if (result)
    {
        *0x2000001a &= 0xfb;
        uint32_t r0_4;
        int32_t r1_1;
        int32_t r2_1;
        r0_4 = sub_15a0();
        uint32_t r0_6;
        
        if (*0x20000122 != r0_4)
            r0_6 = *0x20000122;
        
        if (*0x20000122 == r0_4 || r0_6 <= r0_4)
        {
            result = *0x20000148;
            
            if (result != *0x20000144)
            {
                int32_t r0_9 = *0x20000144;
                *0x20000148 = r0_9;
                result = sub_534c(r0_9, 0x20000120, r2_1);
            }
        }
        else
        {
            sub_534c(r0_6, r1_1, r2_1);
            *0x20000122 = r0_4;
            *0x20000148 = *0x20000144;
            result = sub_1d30(r0_4);
        }
        
        if (r0_4 <= 1)
        {
            *0x2000002e += 1;
            result = *0x2000002e;
            
            if (result > 0x32)
            {
                *0x2000002e = 0;
                
                if (*0x2000001b >> 7)
                {
                    *0x2000001b &= 0x7f;
                    return sub_1d30(0);
                }
                
                *0x2000001b = (*0x2000001b & 0x7f) - 0x80;
                return sub_1d30(1);
            }
        }
    }
    
    return result;
}

int32_t sub_43e8(int32_t arg1, int32_t arg2)
{
    int32_t result = (*0x4002102c & 0xffff03ff) | arg1 | arg2;
    *0x4002102c = result;
    return result;
}

int32_t sub_441c(int32_t arg1)
{
    int32_t result = *0x4002102c >> 4 << 4 | arg1;
    *0x4002102c = result;
    return result;
}

int32_t sub_4438(int32_t arg1, int32_t arg2)
{
    int32_t r3_1 = *0x4002102c & 0xfffffe0f;
    int32_t result;
    
    result = !arg2 ? r3_1 & 0xfffffeff : r3_1 | arg1;
    
    *0x4002102c = result;
    return result;
}

int32_t sub_446c(int32_t arg1)
{
    int32_t result = (*0x40021004 & 0xffffff0f) | arg1;
    *0x40021004 = result;
    return result;
}

void sub_448c(int32_t arg1)
{
    *0x40021000 &= 0xfffeffff;
    *0x40021000 &= 0xfffbffff;
    
    if (arg1 == __builtin_memcpy)
        *0x40021000 |= __builtin_memcpy;
    else if (arg1 == 0x40000)
        *0x40021000 |= 0x50000;
}

void sub_44e4(int32_t arg1)
{
    *0x40021000 = *0x40021000 >> 1 << 1;
    
    if (arg1 == 1)
        *0x40021000 |= 1;
}

void sub_450c(int32_t arg1)
{
    *0x4002102c &= 0xff1fffff;
    *0x4002102c |= arg1;
}

int32_t sub_4528(int32_t arg1)
{
    int32_t result = (*0x40021004 & 0xfffff8ff) | arg1;
    *0x40021004 = result;
    return result;
}

int32_t sub_454c(int32_t arg1)
{
    int32_t result = (*0x40021004 & 0xffffc7ff) | arg1 << 3;
    *0x40021004 = result;
    return result;
}

void sub_456c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    *0x40021004 &= 0xff3fffff;
    int32_t r1_1 = *0x40021004 & 0xfec0ffff;
    int32_t r1_2;
    
    if (!arg1 || arg1 == 0x1000000)
        r1_2 = r1_1 | arg1 | arg2 | arg3 | arg4;
    else
    {
        r1_2 = r1_1 | arg4;
        *0x40021000 |= 0x400000;
    }
    
    *0x40021004 = r1_2;
}

int32_t sub_45c4(int32_t arg1)
{
    int32_t result = *0x40021004 >> 3 << 3 | arg1;
    *0x40021004 = result;
    return result;
}

int32_t sub_45e0()
{
    *0x40021000 |= 1;
    *0x40021004 &= 0x21ff0000;
    *0x40021000 &= 0xfef6ffff;
    *0x40021000 &= 0xffbfffff;
    *0x40021004 &= 0xfe00ffff;
    *0x4002102c = 0x3800;
    *0x40021008 = 0xbf0000;
    return 0xbf0000;
}

void sub_463c(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x40021014 &= ~arg1;
    else
        *0x40021014 |= arg1;
}

void sub_465c(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x40021028 &= ~arg1;
    else
        *0x40021028 |= arg1;
}

void sub_467c(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x4002101c &= ~arg1;
    else
        *0x4002101c |= arg1;
}

void sub_469c(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x40021010 &= ~arg1;
    else
        *0x40021010 |= arg1;
}

void sub_46bc(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x40021018 &= ~arg1;
    else
        *0x40021018 |= arg1;
}

// SPI1 CLOCK ENABLE

void sub_46dc(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *0x4002100c &= ~arg1;
    else
        *0x4002100c |= arg1;
}

void sub_46fc(int32_t arg1)
{
    if (arg1 != 1)
        *0x40021020 = *0x40021020 >> 1 << 1;
    else
        *0x40021020 |= 1;
}

void sub_4720(int32_t arg1)
{
    if (arg1 != 1)
        *0x40021000 &= 0xfeffffff;
    else
        *0x40021000 |= arg1 << 0x18;
}

int32_t sub_4748(int32_t arg1)
{
    int32_t r2 = arg1 >> 5;
    int32_t r3_1;
    
    if (r2 == 1)
        r3_1 = *0x40021000;
    else if (r2 != 2)
        r3_1 = *0x40021024;
    else
        r3_1 = *0x40021020;
    
    if (!(1 << arg1 << 0x1b >> 0x1b & r3_1))
        return 0;
    
    return 1;
}

int32_t sub_4788()
{
    return *0x40021004 & 0xc008;
}

int32_t sub_479c()
{
    int32_t r3;
    int32_t var_10_1 = r3;
    int32_t var_10 = 0;
    int32_t i;
    
    do
    {
        i = sub_4748(0x31);
        var_10 += 1;
        
        if (var_10 == 0x2000)
            break;
    } while (!i);
    
    if (!sub_4748(0x31))
        return 0;
    
    return 1;
}

int32_t sub_47d8()
{
    int32_t r3;
    int32_t var_10_1 = r3;
    int32_t var_10 = 0;
    int32_t i;
    
    do
    {
        i = sub_4748(0x21);
        var_10 += 1;
        
        if (var_10 == 0x500)
            break;
    } while (!i);
    
    if (!sub_4748(0x21))
        return 0;
    
    return 1;
}

int32_t sub_4814(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t var_10 = arg3;
    sub_748c(&var_10, 0x3ff000, 5);
    int32_t result;
    
    if (arg4 != 0xbb)
    {
        result = 0;
        *0x20000144 = 0;
    }
    else
    {
        *0x20000144 = *var_10[3];
        *0x20000144 = *0x20000144 << 8 | *var_10[2];
        *0x20000144 = *0x20000144 << 8 | *var_10[1];
        result = *0x20000144 << 8 | var_10;
        *0x20000144 = result;
    }
    
    return result;
}

uint32_t sub_4870()
{
    sub_1308();
    sub_2f64();
    return sub_3f84();
}

uint32_t sub_4884()
{
    uint32_t result = *0x2000001a << 0x1a >> 0x1f;
    
    if (result)
    {
        result = *0x2000001a & 0xffffffdf;
        *0x2000001a = result;
    }
    
    return result;
}

int32_t sub_48a4(int32_t arg1, int32_t arg2, int32_t arg3)
{
    int32_t r3;
    int32_t var_18 = r3;
    sub_5c24();
    *0x20000098 = 0;
    *0x200000a4 = arg2;
    *0x20000088 = sub_f4(arg3, 0xffff);
    
    if (!*0x20000088)
    {
        sub_5c24();
        sub_525c(arg2, arg1, arg3);
        return sub_52d0();
    }
    
    *0x20000090 = 0xffff;
    *0x20000094 = sub_f4(arg3, *0x20000090);
    
    for (uint32_t i = 0; i < *0x20000088; i = i + 1)
    {
        sub_5c24();
        sub_525c(i * *0x20000090 + arg2, i * *0x20000090 + arg1, *0x20000090);
        sub_52d0();
    }
    
    int32_t result = *0x20000094;
    
    if (!result)
        return result;
    
    sub_5c24();
    int32_t r0_12 = *0x20000088 * *0x20000090 + arg2;
    var_18 = r0_12;
    sub_525c(r0_12, *0x20000088 * *0x20000090 + arg1, *0x20000094);
    return sub_52d0();
}

int32_t sub_4978(int32_t arg1, int32_t arg2, int32_t arg3)
{
    sub_463c(1, 1);
    int32_t var_3c;
    
    if (arg3 != 1)
    {
        sub_188c(0x40020030);
        var_3c = 0x4001440c;
        int32_t var_34_2 = 0;
        int32_t var_30_2 = arg2;
        int32_t var_2c_2 = 0;
        int32_t var_38_2 = arg1;
        int32_t var_28_1 = 0x80;
        int32_t var_24_1 = 0;
        int32_t var_20_1 = 0;
        int32_t var_1c_1 = 0;
        int32_t var_18_1 = 0x2000;
        int32_t var_14_1 = 0;
        sub_1c70(0x40020030, &var_3c);
        return sub_1cb0(0x10, 0x40020000, 0x40020030, 1);
    }
    
    sub_188c(0x40020030);
    sub_188c(0x40020044);
    var_3c = 0x4001440c;
    int32_t var_38 = 0x200000c4;
    int32_t var_34 = 0x10;
    int32_t var_30 = 0x200;
    int32_t var_2c_1;
    __builtin_memset(&var_2c_1, 0, 0x14);
    int32_t var_14 = 0;
    int32_t var_18 = arg3 << 0xd;
    sub_1c70(0x40020044, &var_3c);
    sub_1cb0(0xf, 0x40020000, 0x40020044, 1);
    var_3c = 0x4001440c;
    int32_t var_38_1 = 0x2000018a;
    int32_t var_34_1 = 0;
    int32_t var_28 = 0x80;
    int32_t var_30_1 = 0x200;
    int32_t var_1c = 0;
    sub_1c70(0x40020030, &var_3c);
    return sub_1cb0(0x10, 0x40020000, 0x40020030, 1);
}

void sub_4a5c(int16_t* arg1, int32_t arg2)
{
    if (!arg2)
        *arg1 &= 0xffbf;
    else
        *arg1 |= 0x40;
}

int32_t sub_4a78()
{
    sub_741c();
    sub_2dbc(0x40010800, 0x100);
    *0x20000178 = 0;
    *0x2000017a = 0x104;
    *0x2000017c = 0;
    *0x2000017e = 2;
    *0x20000180 = 1;
    *0x20000182 = 0x200;
    *0x20000184 = 8;
    *0x20000186 = 0;
    *0x20000188 = 7;
    sub_4b68(0x40014400, 0x20000178);
    sub_4a5c(0x40014400, 1);
    return 0x40014400;
}

// SPI1 PRE SETUP?

int32_t sub_4acc(int32_t arg1)
{
    int32_t result;
    
    if (arg1 != 0x40012000)
    {
        result = 0x40014400;
        
        if (arg1 == 0x40014400)
        {
            sub_46dc(0x400, 1);
            result = arg1 >> 0x14;
            sub_46dc(result, 0);
        }
    }
    else
    {
        sub_46dc(0x200, 1);
        result = arg1 >> 0x15;
        sub_46dc(result, 0);
    }
    
    return result;
}

void sub_4b08(void* arg1, int32_t arg2, int32_t arg3)
{
    if (!arg3)
        *(arg1 + 4) &= ~arg2;
    else
        *(arg1 + 4) |= arg2;
}

int32_t sub_4b1c(void* arg1, int32_t arg2)
{
    int32_t result;
    
    if (!(*(arg1 + 8) & arg2))
        result = 0;
    else
        result = 1;
    
    if (*0x20000058 <= 0x50)
        return result;
    
    __dsb_SY();
    *0xe000ed0c = 0x5fa0004;
    __dsb_SY();
    
    while (true)
        /* nop */
}

uint32_t sub_4b5c(void* arg1)
{
    return *(arg1 + 0xc);
}

void sub_4b62(void* arg1, int16_t arg2)
{
    *(arg1 + 0xc) = arg2;
}

void sub_4b68(int16_t* arg1, int16_t* arg2)
{
    *arg1 = (*arg1 & 0x3040) | *arg2 | arg2[1] | arg2[2] | arg2[3] | arg2[4] | arg2[5] | arg2[6]
        | arg2[7];
    arg1[0xe] &= 0xf7ff;
    arg1[8] = arg2[8];
}

uint32_t sub_4ba8()
{
    uint32_t result = 0;
    
    for (uint32_t i = 0; i < 8; i = i + 1)
    {
        result = result << 0x19 >> 0x18;
        sub_2db8(0x40010c00, 8);
        
        if (sub_2da4(0x40010c00, 0x20) == 1)
            result = result + 1;
        
        sub_2dbc(0x40010c00, 8);
    }
    
    return result;
}

int32_t sub_4bf4() __pure
{
    return;
}

int32_t sub_4bf8()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 0x2000;
    int32_t var_10 = 0;
    int32_t var_14 = 1;
    int32_t var_18 = 2;
    int32_t var_8 = 0;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x4000;
    int32_t var_14_1 = 2;
    return sub_2ba0(0x40010800, &var_1c);
}

int32_t sub_4c3c()
{
    int32_t i = 0;
    *0x40021000 |= 1;
    
    do
    {
        i += 1;
        
        if ((*0x40021000 & 2) >> 1)
            break;
    } while (i != 0x500);
    
    if ((*0x40021000 & 2) >> 1)
    {
        *0x40022000 = *0x40022000 >> 2 << 2;
        *0x40022000 |= 2;
        *0x40021004 = *0x40021004;
        *0x40021004 &= 0xff3fffff;
        *0x40021004 &= 0xfec0ffff;
        int32_t result = *0x40021004 | 0x30000;
        *0x40021004 = result;
        *0x40021000 |= 0x1000000;
        
        while (!(*0x40021000 & 0x2000000))
            /* nop */
        
        *0x40021004 = *0x40021004 >> 3 << 3;
        *0x40021004 |= 2;
        
        while ((*0x40021004 & 0xc008) != 0x4000)
            /* nop */
        
        return result;
    }
    
    while (true)
        /* nop */
}

void sub_4d24(int16_t arg1)
{
    *0x20000008 = arg1;
}

int32_t sub_4d30()
{
    return sub_62d0();
}

int32_t sub_4d38(int16_t arg1)
{
    sub_59c6(0x40013400, arg1);
    *0x20000128 = 0;
    return 0;
}

int32_t sub_4d54(int16_t arg1)
{
    sub_59ca(0x40012c00, arg1);
    *0x20000128 = 0;
    return 0;
}

int32_t sub_4d70()
{
    sub_b24(0x40020800);
    sub_5680(0x40001000);
    sub_33e8();
    sub_73b8();
    int32_t var_1c = 0xc000;
    int32_t var_10 = 0;
    int32_t var_14 = 2;
    int32_t var_18 = 1;
    sub_2ba0(0x40011000, &var_1c);
    sub_2db8(0x40011000, 0xc000);
    var_1c = 0x11;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x11);
    var_1c = 0xf00;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0xf00);
    var_1c = 0x8000;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x8000);
    var_1c = 0xe8;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 0xe8);
    var_1c = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 1);
    var_1c = 0x1000;
    int32_t var_14_1 = 0;
    int32_t var_18_1 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x1000);
    var_1c = 0x10;
    int32_t var_14_2 = 0;
    int32_t var_18_2 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2dbc(0x40010c00, 0x10);
    var_1c = 0x100;
    int32_t var_10_1 = 0;
    int32_t var_14_3 = 2;
    int32_t var_18_3 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 0x100);
    var_1c = 0x20;
    int32_t var_10_2 = 0;
    int32_t var_14_4 = 2;
    int32_t var_18_4 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x20);
    var_1c = 0x6000;
    int32_t var_10_3 = 0;
    int32_t var_14_5 = 0;
    int32_t var_18_5 = 0;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x40;
    int32_t var_10_4 = 0;
    int32_t var_14_6 = 0;
    int32_t var_18_6 = 0x11;
    sub_2ba0(0x40010800, &var_1c);
    sub_2dbc(0x40010800, 0x40);
    sub_2d88(&var_1c);
    var_1c = 6;
    int32_t var_10_5 = 0;
    int32_t var_14_7 = 2;
    int32_t var_18_7 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 6);
    var_1c = 4;
    int32_t var_10_6 = 0;
    int32_t var_18_8 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 4);
    sub_5680(0x40000400);
    sub_5680(0x40012c00);
    return sub_5680(0x40013400);
}

uint32_t sub_4f44()
{
    int32_t r3;
    int32_t var_8 = r3;
    uint32_t result;
    
    if (*0x20000150)
    {
        result = *0x20000027;
        
        if (result > 2)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 <= 0x29)
            {
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x240a60);
                *0x20000026 += 1;
            }
            else
            {
                *0x20000026 = 0;
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x240a60);
                *0x20000026 += 1;
            }
            
            result = *0x2000001b << 0x1e >> 0x1f;
            
            if (!result)
            {
                result = (*0x2000001b & 0xfffffffd) + 2;
                *0x2000001b = result;
            }
        }
    }
    else
    {
        result = *0x20000027;
        
        if (result > 2)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 <= 0x29)
            {
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x9ca60);
                *0x20000026 += 1;
            }
            else
            {
                *0x20000026 = 0;
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x9ca60);
                *0x20000026 += 1;
            }
            
            result = *0x2000001b << 0x1e >> 0x1f;
            
            if (!result)
            {
                result = (*0x2000001b & 0xfffffffd) + 2;
                *0x2000001b = result;
            }
        }
    }
    
    return result;
}

uint32_t sub_5074()
{
    if (*0x20000019 << 0x1c >> 0x1f)
        return sub_3f84();
    
    uint32_t result = *0x20000019 << 0x1b >> 0x1f;
    
    if (!result)
    {
        *0x20000019 = (*0x20000019 & 0xef) + 0x10;
        *0x200000cc = 0;
        *0x2000005a = 0;
        sub_5b0c(sub_1480());
        uint32_t r0_9 = *0x20000150;
        
        if (r0_9)
        {
            *0x20000110 = sub_3844();
            *0x20000114 = sub_39b0();
            uint32_t r0_29;
            int32_t r1_5;
            r0_29 = sub_698(*0x20000110);
            uint32_t r0_31;
            int32_t r1_6;
            
            if (r0_9 > 0)
                r0_31 = sub_698(*0x20000114);
            
            if (r0_9 <= 0 || r0_9 <= 0)
            {
                sub_1518();
                return sub_3f84();
            }
            
            uint32_t r0_33;
            int32_t r1_7;
            r0_33 = sub_698(*0x20000110);
            uint32_t r0_35;
            int32_t r1_8;
            
            if (r0_9 > 0)
                r0_35 = sub_698(*0x20000114);
            
            if (r0_9 <= 0 || r0_9 <= 0)
            {
                sub_1518();
                return sub_3f84();
            }
            
            sub_4d30();
            *0x2000001d = (*0x2000001d & 0xfd) + 2;
            *0x20000128 = 0;
            *0x20000124 = 0;
            sub_1330();
            sub_133c();
            *0x2000001b &= 0xfd;
            *0x20000019 = (*0x20000019 & 0xdf) + 0x20;
            result = *0x2000001b & 0xfffffff7;
            *0x2000001b = result;
        }
        else
        {
            *0x20000110 = sub_3844();
            *0x20000114 = sub_39b0();
            uint32_t r0_13;
            int32_t r1_1;
            r0_13 = sub_698(*0x20000110);
            uint32_t r0_15;
            int32_t r1_2;
            
            if (r0_9 > 0)
                r0_15 = sub_698(*0x20000114);
            
            if (r0_9 <= 0 || r0_9 <= 0)
            {
                sub_1518();
                return sub_3f84();
            }
            
            uint32_t r0_17;
            int32_t r1_3;
            r0_17 = sub_698(*0x20000110);
            uint32_t r0_19;
            int32_t r1_4;
            
            if (r0_9 > 0)
                r0_19 = sub_698(*0x20000114);
            
            if (r0_9 <= 0 || r0_9 <= 0)
            {
                sub_1518();
                return sub_3f84();
            }
            
            sub_4d30();
            *0x20000128 = 0;
            *0x20000124 = 0;
            sub_1330();
            sub_133c();
            *0x2000001b &= 0xfd;
            *0x20000019 = (*0x20000019 & 0xdf) + 0x20;
            result = *0x2000001b & 0xfffffff7;
            *0x2000001b = result;
        }
    }
    
    return result;
}

int32_t sub_525c(int32_t arg1, int32_t arg2, int32_t arg3)
{
    sub_2db8(0x40010800, 0x100);
    sub_4978(arg2, arg3, 0);
    sub_74e8(3);
    sub_74e8((0xff0000 & arg1) >> 0x10);
    sub_74e8((0xff00 & arg1) >> 8);
    sub_74e8(arg1);
    sub_4b08(0x40014400, 1, 1);
    sub_192c(0x40020030, 1);
    *0x20000178 = 0x400;
    return sub_4b68(0x40014400, 0x20000178);
}

int32_t sub_52d0()
{
    *0x20000058 = 0;
    int32_t i;
    
    do
        i = sub_1c10(0x200, 0x40020000);
     while (!i);
    sub_2dbc(0x40010800, 0x100);
    *0x40014404 &= 0xfffe;
    *0x40020030 &= 0xfffe;
    *0x20000178 = 0;
    sub_4b68(0x40014400, 0x20000178);
    *0x4001440c;
    return sub_7530();
}

int32_t sub_5338()
{
    *0x20000027 = 0;
    *0x20000026 = 0;
    return 0;
}

uint32_t sub_534c(int32_t arg1, int32_t arg2, int32_t arg3)
{
    int32_t r3;
    int32_t var_c = r3;
    int32_t var_10 = arg3;
    var_10 = *0x20000144;
    *var_10[1] = (*0x20000144 & 0xff00) >> 8;
    *var_10[2] = (*0x20000144 & 0xff0000) >> 0x10;
    *var_10[3] = *0x20000144 >> 0x18;
    var_c = 0xbb;
    sub_736c(0x3ff000);
    return sub_7568(&var_10, 0x3ff000, 5);
}

int32_t sub_53a0() __pure
{
    return;
}

int32_t sub_53a4()
{
    sub_3084();
    *0x40021000 |= 1;
    *0x40021004 &= 0x21ff0000;
    *0x40021000 &= 0xfef6ffff;
    *0x40021000 &= 0xffbfffff;
    *0x40021004 &= 0xfe00ffff;
    *0x4002102c = 0x3800;
    *0x40021008 = 0xbf0000;
    *0x40022000 |= 0x10;
    return sub_4c3c();
}

uint32_t sub_541c()
{
    uint32_t result = *0x200000c9;
    
    if (!result)
    {
        result = *0x40010c10 & 2;
        
        if (!result)
            return sub_7dd8();
    }
    
    return result;
}

uint32_t sub_5440()
{
    uint32_t result = *0x20000019 >> 7;
    
    if (result)
    {
        *0x20000019 &= 0x7f;
        result = *0x20000006;
        
        if (result <= 0xe)
            return (*(0x8007e38 + (*0x20000006 << 2)))();
    }
    
    return result;
}

uint32_t sub_547c()
{
    uint32_t result = *0x2000001a << 0x1e >> 0x1f;
    
    if (result)
    {
        result = *0x2000004c;
        
        if (result > 0x1f4)
        {
            *0x2000004c = 0;
            sub_3d5c();
            /* no return */
        }
    }
    
    return result;
}

int32_t sub_54ac()
{
    *0x2000005c = 0xa64;
    int16_t var_14 = 8;
    int16_t var_12 = 0;
    int16_t var_10 = *0x2000005c;
    int16_t var_e = 0;
    char var_c = 0;
    sub_5898(0x40012c00, &var_14);
    int16_t var_24;
    sub_5884(&var_24);
    var_24 = 0x70;
    int16_t var_22 = 1;
    int16_t var_1e = 0;
    int16_t var_1c = 2;
    sub_57ec(0x40012c00, &var_24);
    sub_56f4(0x40012c00, 1);
    sub_570c(0x40012c00, 1);
    sub_59ca(0x40012c00, 0);
    return 0x40012c00;
}

int32_t sub_5520()
{
    *0x2000005c = 0xfa0;
    int16_t var_14 = 8;
    int16_t var_12 = 0;
    int16_t var_10 = *0x2000005c;
    int16_t var_e = 0;
    char var_c = 0;
    sub_5898(0x40000400, &var_14);
    int16_t var_24;
    sub_5884(&var_24);
    var_24 = 0x70;
    int16_t var_22 = 1;
    int16_t var_1e = 0;
    int16_t var_1c = 0;
    sub_5764(0x40000400, &var_24);
    sub_56f4(0x40000400, 1);
    sub_570c(0x40000400, 1);
    sub_59c6(0x40000400, 0);
    return 0x40000400;
}

int32_t sub_5590(int32_t arg1)
{
    int32_t r1;
    int32_t var_14 = r1;
    int32_t result = arg1;
    var_14 = 0x2f44;
    result = 0;
    *var_14[2] = 0;
    *result[2] = 0;
    sub_5898(0x40001000, &result);
    sub_567a(0x40001000, 0, 1);
    sub_5666(0x40001000, 1, 1);
    sub_56f4(0x40001000, 1);
    return result;
}

int32_t sub_55d0()
{
    *0x2000005c = 0xa64;
    int16_t var_14 = 8;
    int16_t var_12 = 0;
    int16_t var_10 = *0x2000005c;
    int16_t var_e = 0;
    char var_c = 0;
    sub_5898(0x40013400, &var_14);
    int16_t var_24;
    sub_5884(&var_24);
    var_24 = 0x70;
    int16_t var_22 = 1;
    int16_t var_1e = 0;
    int16_t var_1c = 2;
    sub_5764(0x40013400, &var_24);
    sub_56f4(0x40013400, 1);
    sub_570c(0x40013400, 1);
    sub_59c6(0x40013400, 0);
    return 0x40013400;
}

int32_t sub_5644()
{
    sub_4528(0x500);
    sub_467c(0x10, 1);
    sub_46bc(4, 1);
    return 4;
}

void sub_5660(void* arg1, int32_t arg2)
{
    *(arg1 + 0x10) = ~arg2;
}

void sub_5666(void* arg1, int32_t arg2, int32_t arg3)
{
    if (!arg3)
        *(arg1 + 0xc) &= ~arg2;
    else
        *(arg1 + 0xc) |= arg2;
}

void sub_567a(void* arg1, int16_t arg2, int16_t arg3)
{
    *(arg1 + 0x28) = arg2;
    *(arg1 + 0x14) = arg3;
}

int32_t sub_5680(int32_t arg1)
{
    int32_t result;
    
    if (arg1 == 0x40012c00)
    {
        sub_46dc(0x1000, 1);
        result = arg1 >> 0x12;
        sub_46dc(result, 0);
    }
    else if (arg1 == 0x40000400)
    {
        sub_469c(2, 1);
        result = 2;
        sub_469c(2, 0);
    }
    else if (arg1 != 0x40001000)
    {
        result = 0x40013400;
        
        if (arg1 == 0x40013400)
        {
            sub_46dc(0x2000, 1);
            result = arg1 >> 0x11;
            sub_46dc(result, 0);
        }
    }
    else
    {
        sub_469c(0x10, 1);
        result = 0x10;
        sub_469c(0x10, 0);
    }
    
    return result;
}

void sub_56f4(int32_t* arg1, int32_t arg2)
{
    if (!arg2)
        *arg1 = *arg1 >> 1 << 1;
    else
        *arg1 |= 1;
}

void sub_570c(int32_t arg1, int32_t arg2)
{
    if (!arg2)
        *(&data_44 + arg1) = *(&data_44 + arg1) << 0x11 >> 0x11;
    else
        *(&data_44 + arg1) |= 0x8000;
}

int32_t sub_572e(void* arg1, int32_t arg2)
{
    if (*(arg1 + 0x10) & arg2 && *(arg1 + 0xc) & arg2)
        return 1;
    
    return 0;
}

int32_t sub_5754()
{
    int32_t r0_1;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    r0_1 = sub_5590(sub_5644());
    return sub_59b0(r0_1, r1, r2, r3);
}

void sub_5764(void* arg1, int16_t* arg2)
{
    *(arg1 + 0x20) = *(arg1 + 0x20) >> 1 << 1;
    int32_t r3 = *(arg1 + 4);
    int16_t r4_3 = (*(arg1 + 0x18) & 0xff8c) | *arg2;
    int32_t r2_3 = (*(arg1 + 0x20) & 0xfffffffd) | arg2[4] | arg2[1];
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400)
    {
        r2_3 = (((r2_3 & 0xfffffff7) | arg2[5]) & 0xfffffffb) | arg2[2];
        r3 = (r3 & 0xfffffcff) | arg2[6] | arg2[7];
    }
    
    *(arg1 + 4) = r3;
    *(arg1 + 0x18) = r4_3;
    *(arg1 + 0x34) = arg2[3];
    *(arg1 + 0x20) = r2_3;
}

void sub_57ec(void* arg1, int16_t* arg2)
{
    *(arg1 + 0x20) &= 0xfffffeff;
    int32_t r3 = *(arg1 + 4);
    int16_t r4_3 = (*(arg1 + 0x1c) & 0xff8c) | *arg2;
    int32_t r2_3 = (*(arg1 + 0x20) & 0xfffffdff) | arg2[4] << 8 | arg2[1] << 8;
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400)
    {
        r2_3 = (((r2_3 & 0xfffff7ff) | arg2[5] << 8) & 0xfffffbff) | arg2[2] << 8;
        r3 = (r3 & 0xffffcfff) | arg2[6] << 4 | arg2[7] << 4;
    }
    
    *(arg1 + 4) = r3;
    *(arg1 + 0x1c) = r4_3;
    *(arg1 + 0x3c) = arg2[3];
    *(arg1 + 0x20) = r2_3;
}

void sub_5884(int16_t* arg1)
{
    *arg1 = 0;
    arg1[1] = 0;
    arg1[2] = 0;
    arg1[3] = 0;
    arg1[4] = 0;
    arg1[5] = 0;
    arg1[6] = 0;
    arg1[7] = 0;
}

void sub_5898(int32_t* arg1, int16_t* arg2)
{
    int32_t r1 = *arg1;
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400 || arg1 == 0x40000400)
        r1 = (r1 & 0xffffff8f) | arg2[1];
    
    if (arg1 != 0x40001000)
        r1 = (r1 & 0xfffffcff) | arg2[3];
    
    *arg1 = r1;
    arg1[0xb] = arg2[2];
    arg1[0xa] = *arg2;
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400)
        arg1[0xc] = arg2[4];
    
    arg1[5] = 1;
    int32_t r1_3 = *arg1;
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400 || arg1 == 0x40000400)
    {
        if (!*(arg2 + 9))
            r1_3 &= 0xfffff7ff;
        else
            r1_3 |= 0x800;
    }
    
    if (arg1 == 0x40000400)
    {
        int32_t r1_4;
        
        if (!arg2[5])
            r1_4 = r1_3 & 0xffffefff;
        else
            r1_4 = r1_3 | arg1 >> 0x12;
        
        if (!*(arg2 + 0xb))
            r1_3 = r1_4 & 0xffffdfff;
        else
            r1_3 = r1_4 | 0x2000;
    }
    
    if (arg1 == 0x40012c00 || arg1 == 0x40013400 || arg1 == 0x40000400)
    {
        if (!arg2[6])
            r1_3 &= 0xffff7fff;
        else
            r1_3 |= 0x8000;
    }
    
    *arg1 = r1_3;
    int32_t r1_5 = arg1[1];
    
    if (arg1 == 0x40000400)
    {
        if (!*(arg2 + 0xd))
            r1_5 &= 0xfffffeff;
        else
            r1_5 |= arg1 >> 0x16;
    }
    
    arg1[1] = r1_5;
}

int32_t sub_59b0(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t var_8 = arg4;
    var_8 = 0x14;
    *var_8[1] = 2;
    *var_8[2] = 1;
    return sub_3c84(&var_8);
}

void sub_59c6(void* arg1, int16_t arg2)
{
    *(arg1 + 0x34) = arg2;
}

void sub_59ca(void* arg1, int16_t arg2)
{
    *(arg1 + 0x3c) = arg2;
}

uint32_t sub_59d0()
{
    if (*0x200000d6 == 1)
    {
        *0x200000d6 = 0;
        sub_eb8();
        sub_37a4();
    }
    
    return sub_59f4();
}

uint32_t sub_59f4()
{
    uint32_t result = *0x200000d7;
    bool cond:1 = result >= 1;
    
    if (result == 1)
    {
        *0x200000d7 = 0;
        uint32_t r0_1;
        int32_t r1_1;
        r0_1 = sub_698(*0x2000013c);
        int32_t var_14_1 = r1_1;
        uint32_t var_18_1 = r0_1;
        
        if (result >= 1)
        {
            uint32_t r0_3;
            int32_t r1_2;
            r0_3 = sub_698(*0x2000013c);
            int32_t var_14_2 = r1_2;
            uint32_t var_18_2 = r0_3;
            result = sub_6e8(r0_3, r1_2, 0x7ae147ae, 0x400bae14);
            
            if (!cond:1)
            {
                uint32_t r0_5;
                int32_t r1_3;
                r0_5 = sub_698(*0x2000013c);
                int32_t var_24_1 = r1_3;
                uint32_t var_28_1 = r0_5;
                int32_t r0_6;
                int32_t r1_4;
                r0_6 = sub_43a(r0_5, r1_3, 0x7ae147ae, 0x400bae14);
                int32_t var_1c_1 = r1_4;
                int32_t var_20_1 = r0_6;
                int32_t r0_7;
                int32_t r1_5;
                r0_7 = sub_528(r0_6, r1_4, 0x47ae147b, 0x3f747ae1);
                int32_t var_14_3 = r1_5;
                int32_t var_18_3 = r0_7;
                uint32_t r4_1 = sub_65c(r0_7, r1_5);
                result = *0x200000c9;
                
                if (!result)
                {
                    if (*0x40010c10 & 2)
                    {
                        if (*0x2000011d <= r4_1)
                        {
                            result = *0x2000001a >> 7;
                            
                            if (!result)
                            {
                                *0x2000011d = r4_1;
                                result = (*0x2000001a & 0xffffff7f) + 0x80;
                                *0x2000001a = result;
                            }
                        }
                        else
                        {
                            result = 0x20000110;
                            *0x2000011d = r4_1;
                        }
                    }
                    else if (*0x2000011d <= r4_1)
                    {
                        result = *0x2000011d;
                        
                        if (result < r4_1)
                        {
                            result = 0x20000110;
                            *0x2000011d = r4_1;
                        }
                    }
                    else
                    {
                        result = *0x2000001d << 0x1e >> 0x1f;
                        
                        if (result)
                        {
                            *0x2000011d = r4_1;
                            result = *0x2000001d & 0xfffffffd;
                            *0x2000001d = result;
                        }
                    }
                }
            }
        }
        else
        {
            result = 0x64;
            *0x2000011d = 0x64;
        }
    }
    
    return result;
}

int32_t sub_5b0c(int32_t arg1)
{
    int32_t r3;
    int32_t var_c_1 = r3;
    int32_t r2;
    int32_t var_10_1 = r2;
    int32_t r1;
    int32_t var_14_1 = r1;
    int32_t result = arg1;
    result = *0x20000130;
    int32_t var_14 = *0x20000130;
    int32_t var_10 = *0x20000130;
    int32_t var_c = *0x20000130;
    *0x20000040 = sub_612c(&result, 4);
    return result;
}

int32_t sub_5b50(int32_t arg1)
{
    int32_t r4 = arg1;
    sub_2dbc(0x40010800, 0x8000);
    sub_2dbc(0x40010c00, 0x20);
    sub_2dbc(0x40010c00, 8);
    sub_2dbc(0x40010c00, 0x80);
    sub_2db8(0x40010800, 0x8000);
    sub_2db8(0x40010c00, 0x80);
    
    for (int32_t i = 0; i < 8; i += 1)
    {
        if (!(0x80 & r4))
            sub_2db8(0x40010c00, 0x20);
        else
            sub_2dbc(0x40010c00, 0x20);
        
        r4 <<= 1;
        sub_2db8(0x40010c00, 8);
        sub_2dbc(0x40010c00, 8);
    }
    
    return 0x40010c00;
}

int32_t sub_5bcc()
{
    int32_t i_1 = 0;
    int32_t i;
    
    do
    {
        if (!sub_2dc0())
            break;
        
        i = i_1 + 1;
        i_1 = i;
    } while (i < 0x14);
    return 0xff;
}

int32_t sub_5bf8()
{
    *0x20000058 = 0;
    int32_t i;
    
    do
        i = sub_1c10(0x20000, 0x40020000);
     while (!i);
    int32_t i_1;
    
    do
        i_1 = sub_2dd8();
     while (i_1);
    
    return i_1;
}

int32_t sub_5c24()
{
    int32_t i_1 = 0;
    int32_t i;
    
    do
    {
        if (!sub_2df0())
            break;
        
        i = i_1 + 1;
        i_1 = i;
    } while (i < 0x14);
    return 0xff;
}

int32_t sub_5c50()
{
    uint32_t r0_1;
    int32_t r1;
    r0_1 = sub_698(*0x2000013c);
    int32_t result;
    bool z;
    bool c;
    
    if (!z && c)
    {
        uint32_t r0_3;
        int32_t r1_1;
        r0_3 = sub_698(*0x2000013c);
        
        if (!z && c)
        {
            uint32_t r0_5;
            int32_t r1_2;
            r0_5 = sub_698(*0x2000013c);
            
            if (!z && c)
            {
                uint32_t r0_7;
                int32_t r1_3;
                r0_7 = sub_698(*0x2000013c);
                result = 3;
                *0x20000120 = 3;
            }
            else
            {
                result = 4;
                *0x20000120 = 4;
            }
        }
        else
        {
            result = 5;
            *0x20000120 = 5;
        }
    }
    else
    {
        result = 6;
        *0x20000120 = 6;
    }
    
    return result;
}

int32_t sub_5d48(uint32_t arg1) __pure
{
    int32_t r1 = 0x20;
    uint32_t r2 = arg1 >> 0x10;
    
    if (r2)
    {
        r1 = 0x10;
        arg1 = r2;
    }
    
    uint32_t r2_1 = arg1 >> 8;
    
    if (r2_1)
    {
        arg1 = r2_1;
        r1 -= 8;
    }
    
    uint32_t r2_2 = arg1 >> 4;
    
    if (r2_2)
    {
        arg1 = r2_2;
        r1 -= 4;
    }
    
    uint32_t r2_3 = arg1 >> 2;
    
    if (r2_3)
    {
        arg1 = r2_3;
        r1 -= 2;
    }
    
    if (!(arg1 >> 1))
        return r1 - arg1;
    
    return r1 - 2;
}

int32_t sub_5d76(int32_t, int32_t, int32_t, uint32_t arg4)
{
    int32_t lr;
    uint32_t r5 = *(lr - 1);
    
    if (arg4 < r5)
        r5 = arg4;
    
    /* jump -> lr + (*(lr + r5) << 1) */
}

uint32_t sub_5db0(int16_t* arg1, int32_t arg2)
{
    int32_t r3;
    int32_t var_18_1 = r3;
    int32_t r2;
    int32_t var_1c_1 = r2;
    int32_t var_20_2 = arg2;
    uint32_t i = 0;
    uint32_t var_18 = 0;
    int32_t var_1c = 0;
    int32_t var_20 = 0;
    
    for (; i < arg2; i = i + 1)
    {
        for (uint32_t j = 0; arg2 - 1 - i > j; j = j + 1)
        {
            if (arg1[j] > arg1[j + 1])
            {
                var_18 = arg1[j];
                arg1[j] = arg1[j + 1];
                arg1[j + 1] = var_18;
            }
        }
    }
    
    for (uint32_t i_1 = 1; arg2 - 1 > i_1; i_1 = i_1 + 1)
        var_1c += arg1[i_1];
    
    return sub_f4(var_1c, arg2 - 2);
}

int32_t sub_5e2c()
{
    int32_t result;
    
    if (*0x20000150)
    {
        uint32_t r0_9 = sub_2138() << 1;
        *0x20000134 = *0x2000012c * r0_9;
        
        if (*0x20000134 > 0x30d40)
        {
            if (!*0x20000152)
            {
                *0x20000154 = sub_61a8(0x40533333);
                sub_4d38(*0x20000154);
            }
            else
            {
                *0x20000154 = sub_61a8(0x40400000);
                sub_4d38(*0x20000154);
            }
        }
        
        uint32_t r0_18 = sub_2144() << 1;
        *0x20000138 = *0x2000012c * r0_18;
        result = *0x20000138;
        
        if (result > 0x30d40)
        {
            if (!*0x20000152)
            {
                *0x20000156 = sub_61a8(0x40533333);
                return sub_4d54(*0x20000156);
            }
            
            *0x20000156 = sub_61a8(0x40600000);
            return sub_4d54(*0x20000156);
        }
    }
    else if (*0x20000151)
    {
        uint32_t r0_6 = sub_2144() << 1;
        *0x20000134 = *0x2000012c * r0_6;
        result = *0x20000134;
        
        if (result > 0x30d40)
            return sub_62d0();
    }
    else
    {
        uint32_t r0_3 = sub_2138() << 1;
        *0x20000134 = *0x2000012c * r0_3;
        result = *0x20000134;
        
        if (result > 0x30d40)
            return sub_62d0();
    }
    
    return result;
}

int32_t sub_5f20()
{
    sub_46bc(0x301d, 1);
    sub_4bf8();
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 6;
    int32_t var_18 = 3;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 4;
    int32_t var_18_1 = 3;
    sub_2ba0(0x40010c00, &var_1c);
    int32_t var_18_2 = 0;
    int32_t var_10 = 0;
    var_1c = 1;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x20;
    int32_t var_10_1 = 0;
    int32_t var_18_3 = 0x11;
    sub_2ba0(0x40010800, &var_1c);
    sub_2dbc(0x40010800, 0x20);
    var_1c = 0x10;
    int32_t var_10_2 = 0;
    int32_t var_18_4 = 0x11;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2dbc(0x40010c00, 0x10);
    var_1c = 0x1000;
    int32_t var_10_3 = 0;
    int32_t var_18_5 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2dbc(0x40010800, 0x1000);
    var_1c = 0x1000;
    int32_t var_10_4 = 0;
    int32_t var_18_6 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2dbc(0x40010800, 0x10);
    int32_t var_18_7 = 0;
    int32_t var_10_5 = 0;
    int32_t var_14 = 1;
    var_1c = 6;
    sub_2ba0(0x40010c00, &var_1c);
    int32_t var_18_8 = 0;
    int32_t var_10_6 = 0;
    var_1c = 0xc000;
    sub_2ba0(0x40011000, &var_1c);
    var_1c = 1;
    int32_t var_10_7 = 0;
    int32_t var_18_9 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_402c();
    sub_2d88(&var_1c);
    int32_t var_18_10 = 0;
    int32_t var_10_8 = 0;
    int32_t var_14_1 = 2;
    var_1c = 8;
    sub_2ba0(0x40010800, &var_1c);
    int32_t var_18_11 = 0;
    int32_t var_10_9 = 0;
    int32_t var_14_2 = 1;
    var_1c = 4;
    sub_2ba0(0x40010c00, &var_1c);
    var_1c = 0x80;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x10;
    int32_t var_10_10 = 0;
    int32_t var_18_12 = 1;
    sub_2ba0(0x40010800, &var_1c);
    sub_2db8(0x40010800, 0x10);
    return sub_15d0();
}

int32_t sub_6094()
{
    sub_2ec4();
    return 0xaaaa;
}

uint32_t sub_609c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    uint32_t result;
    
    if (*0x2000000e < 2 || !(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x2000000e;
        
        if (result > 0xa)
        {
            result = *0x20000065;
            
            if (result == 0xd)
            {
                *0x2000012a = 0;
                *0x20000006 = 0;
                result = *0x2000001b << 0x1e >> 0x1f;
                
                if (result)
                {
                    sub_3790();
                    sub_2f64();
                    sub_5338();
                    result = *0x2000001b & 0xfffffffd;
                    *0x2000001b = result;
                }
            }
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        sub_1518();
        sub_6470();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

int32_t sub_612c(int32_t* arg1, int32_t arg2)
{
    int32_t r3;
    int32_t var_18_1 = r3;
    int32_t r2;
    int32_t var_1c_1 = r2;
    int32_t var_20_2 = arg2;
    uint32_t i = 0;
    int32_t var_18 = 0;
    int32_t var_1c = 0;
    int32_t var_20 = 0;
    
    for (; i < arg2; i = i + 1)
    {
        for (uint32_t j = 0; arg2 - 1 - i > j; j = j + 1)
        {
            if (arg1[j] > arg1[j + 1])
            {
                var_18 = arg1[j];
                arg1[j] = arg1[j + 1];
                arg1[j + 1] = var_18;
            }
        }
    }
    
    for (uint32_t i_1 = 1; arg2 - 1 > i_1; i_1 = i_1 + 1)
        var_1c += arg1[i_1];
    
    return sub_f4(var_1c, arg2 - 2);
}

uint32_t sub_61a8(int32_t arg1)
{
    int32_t r0_2;
    int32_t r1;
    r0_2 = sub_618(sub_20f8());
    int32_t var_2c = r1;
    int32_t var_30 = r0_2;
    int32_t r0_3;
    int32_t r1_1;
    r0_3 = sub_458(r0_2, r1, 0xc5ac471b, 0x3f5800a7);
    int32_t var_24 = r1_1;
    int32_t var_28 = r0_3;
    int32_t r0_4 = sub_710(r0_3, r1_1);
    int32_t r4;
    bool c;
    
    if (c)
    {
        int32_t r0_7 = sub_1fa(r0_4, r0_4);
        int32_t r0_9 = sub_1fa(arg1, arg1);
        int32_t var_3c_1 = r0_9;
        int32_t r0_10 = sub_274(r0_9, r0_7);
        int32_t var_34_1 = r0_10;
        uint32_t r0_11;
        int32_t r1_6;
        r0_11 = sub_698(r0_10);
        int32_t var_2c_1 = r1_6;
        uint32_t var_30_1 = r0_11;
        int32_t r0_12;
        int32_t r1_7;
        r0_12 = sub_458(r0_11, r1_6, 0xa3d70a4, 0x3ff0a3d7);
        int32_t var_24_1 = r1_7;
        int32_t var_28_1 = r0_12;
        r4 = sub_710(r0_12, r1_7);
    }
    else
        r4 = 0x3f800000;
    
    int32_t r0_14 = sub_1fa(0x45265000, r4);
    int32_t var_24_2 = r0_14;
    uint32_t result = sub_634(r0_14);
    
    if (result <= 0xa65)
        return result;
    
    return 0xa65;
}

uint32_t sub_6250()
{
    uint32_t result;
    
    if (*0x20000150 != 1)
    {
        result = *0x20000006;
        
        if (result != 3)
        {
            result = *0x20000006;
            
            if (result != 2)
            {
                result = *0x20000019 << 0x1a >> 0x1f;
                
                if (!result)
                {
                    result = *0x20000158;
                    
                    if (result > 0x320)
                    {
                        if (*0x20000151)
                        {
                            result = 0;
                            *0x20000151 = 0;
                            *0x20000158 = 0;
                        }
                        else
                        {
                            *0x20000151 = 1;
                            result = 0;
                            *0x20000158 = 0;
                        }
                    }
                }
            }
        }
    }
    else
    {
        result = 0;
        *0x20000158 = 0;
    }
    
    return result;
}

void sub_62b4(int32_t arg1)
{
    *0x2000001c = (*0x2000001c & 0xef) | (arg1 << 4 & 0x10);
}

int32_t sub_62d0()
{
    if (*0x20000150)
    {
        if (!*0x20000152)
        {
            *0x20000154 = sub_61a8(0x40533333);
            *0x20000156 = sub_61a8(0x40533333);
            sub_4d38(*0x20000154);
            return sub_4d54(*0x20000156);
        }
        
        *0x20000154 = sub_61a8(0x40400000);
        *0x20000156 = sub_61a8(0x40600000);
        sub_4d38(*0x20000154);
        return sub_4d54(*0x20000156);
    }
    
    if (*0x20000151)
    {
        if (!*0x20000152)
        {
            *0x20000156 = sub_61a8(0x40533333);
            return sub_4d54(*0x20000156);
        }
        
        *0x20000156 = sub_61a8(0x40600000);
        return sub_4d54(*0x20000156);
    }
    
    if (!*0x20000152)
    {
        *0x20000154 = sub_61a8(0x40533333);
        return sub_4d38(*0x20000154);
    }
    
    *0x20000154 = sub_61a8(0x40600000);
    return sub_4d38(*0x20000154);
}

int32_t sub_639c()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 1;
    int32_t var_10 = 0;
    int32_t var_18 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2dbc(0x40010c00, 1);
    return 0x40010c00;
}

int32_t sub_63cc()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    var_1c = 1;
    int32_t var_10 = 0;
    int32_t var_18 = 1;
    sub_2ba0(0x40010c00, &var_1c);
    sub_2db8(0x40010c00, 1);
    return 0x40010c00;
}

int32_t sub_63fc()
{
    char result;
    
    if (*0x20000012 < 6)
    {
        result = *0x20000012 + 1;
        *0x20000012 = result;
    }
    else
    {
        uint32_t r0_1 = *0x20000120;
        
        if (r0_1 != 6)
        {
            result = *0x20000120;
            *0x20000012 = result;
        }
        else
        {
            uint32_t r0_3;
            int32_t r1_1;
            r0_3 = sub_698(*0x2000013c);
            
            if (r0_1 > 6)
            {
                result = *0x20000120 - 1;
                *0x20000012 = result;
            }
            else
            {
                *0x20000012 = *0x20000120;
                result = (*0x2000001b & 0xdf) + 0x20;
                *0x2000001b = result;
            }
        }
    }
    
    return result;
}

int32_t sub_6470()
{
    *0x200000cc = 0xbae;
    *0x200000c9 = 0;
    return 0;
}

uint32_t sub_648c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    
    if (*0x2000000e >= 2 && *0x20000019 << 0x19 >> 0x1f)
    {
        *0x20000019 &= 0xbf;
        *0x2000012a = 0;
        return sub_6470();
    }
    
    uint32_t result = *0x2000000e;
    
    if (result > 0xa)
    {
        result = *0x20000065;
        
        if (result == 0xd)
        {
            *0x2000012a = 0;
            *0x20000006 = 0;
            result = *0x2000001b << 0x1e >> 0x1f;
            
            if (result)
            {
                sub_3790();
                sub_2f64();
                sub_5338();
                result = *0x2000001b & 0xfffffffd;
                *0x2000001b = result;
            }
        }
    }
    
    return result;
}

uint32_t sub_6518(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    uint32_t result;
    
    if (*0x2000000e < 2 || !(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x2000000e;
        
        if (result > 0xa)
        {
            result = *0x20000065;
            
            if (result == 0xd)
            {
                *0x2000012a = 0;
                *0x20000006 = 0;
                result = *0x2000001b << 0x1e >> 0x1f;
                
                if (result)
                {
                    sub_3790();
                    sub_2f64();
                    sub_5338();
                    result = *0x2000001b & 0xfffffffd;
                    *0x2000001b = result;
                }
            }
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        sub_1518();
        sub_6470();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

int32_t sub_65a8()
{
    sub_59c6(0x40013400, 0);
    *0x20000128 = 0;
    return 0;
}

int32_t sub_65c4()
{
    sub_59ca(0x40012c00, 0);
    *0x20000128 = 0;
    return 0;
}

void sub_65e0(int32_t arg1)
{
    *0x2000006c = arg1;
    
    while (*0x2000006c)
        /* nop */
}

uint32_t sub_65f4(uint32_t arg1) __pure
{
    uint32_t i;
    
    do
    {
        i = arg1;
        arg1 = arg1 - 1;
    } while (i);
    return arg1;
}

int32_t sub_6628()
{
    int32_t result = *0x2000006c;
    
    if (result)
    {
        result = *0x2000006c - 1;
        *0x2000006c = result;
    }
    
    return result;
}

int32_t sub_6640(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5)
{
    int32_t var_18 = arg4;
    int32_t var_1c = arg3;
    int32_t var_24 = arg1;
    int32_t var_28 = arg5;
    int16_t var_2c = arg3;
    return sub_1948(arg1, arg2, var_2c, arg4, &var_2c);
}

int32_t sub_666c(char arg1, char arg2, int32_t arg3, int32_t arg4, int32_t arg5)
{
    int32_t var_18 = arg4;
    sub_2db8(0x40010800, 0x8000);
    sub_1808(arg1, arg2, arg3, arg4);
    sub_34bc(arg5, (arg4 * arg3) << 1);
    sub_2dbc(0x40010800, 0x8000);
    return 0x40010800;
}

int32_t sub_66a8()
{
    int32_t r3;
    int32_t var_8 = r3;
    return sub_666c(0x22, 0x40, 0x3c, 0x20, 0x80080b0);
}

uint32_t sub_66c0()
{
    int32_t r3;
    int32_t var_8 = r3;
    
    if (*0x2000000e >= 1 && *0x20000019 << 0x19 >> 0x1f)
    {
        *0x20000019 &= 0xbf;
        sub_3790();
        sub_66a8();
        return sub_3f84();
    }
    
    uint32_t result = *0x2000000e;
    
    if (result <= 0xbb8)
        return result;
    
    *0x2000012a = 0;
    *0x20000006 = 0;
    *0x20000124 = 0;
    var_8 = 0;
    sub_30a8(0, 0, 0x80, 0xa0, 0);
    sub_5338();
    return sub_2f64();
}

uint32_t sub_6734()
{
    int32_t r3;
    int32_t var_8 = r3;
    uint32_t result;
    
    if (*0x20000150)
    {
        result = *0x20000027;
        
        if (result > 2)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 <= 0x29)
            {
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x240a60);
                result = *0x20000026 + 1;
                *0x20000026 = result;
            }
            else
            {
                *0x20000026 = 0;
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x240a60);
                result = *0x20000026 + 1;
                *0x20000026 = result;
            }
        }
    }
    else
    {
        result = *0x20000027;
        
        if (result > 2)
        {
            *0x20000027 = 0;
            
            if (*0x20000026 <= 0x29)
            {
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x9ca60);
                result = *0x20000026 + 1;
                *0x20000026 = result;
            }
            else
            {
                *0x20000026 = 0;
                sub_6640(0, 0, 0x80, 0xa0, 0xa000 * *0x20000026 + 0x9ca60);
                result = *0x20000026 + 1;
                *0x20000026 = result;
            }
        }
    }
    
    return result;
}

int32_t sub_682c()
{
    int32_t r3;
    int32_t var_8 = r3;
    char result;
    
    if (*0x2000000e < 1 || !(*0x20000019 << 0x19 >> 0x1f))
    {
        if (*0x2000000e > 0x9c4)
        {
            sub_2e1c();
            *0x2000012a = 0;
            *0x20000006 = 0;
            *0x20000124 = 0;
            var_8 = 0;
            sub_30a8(0, 0, 0x80, 0xa0, 0);
            sub_5338();
            return sub_2f64();
        }
        
        if (*0x2000000e < 0x4e2)
        {
            sub_6734();
            result = 0;
            *0x2000012a = 0;
        }
        else
        {
            sub_6734();
            result = 0;
            *0x2000012a = 0;
        }
    }
    else
    {
        result = *0x20000019 & 0xbf;
        *0x20000019 = result;
    }
    
    return result;
}

uint32_t sub_68bc()
{
    uint32_t result;
    
    if (*0x40011010 & 0x4000)
    {
        if (*0x200000e4 < 0xea60)
            *0x200000e4 += 1;
        
        result = *0x200000e4;
        
        if (result > 2)
        {
            *0x200000e4 = 0;
            *0x200000e2 = 0;
            
            if (*0x200000e0 != 1 && *0x200000e0 != 2)
                result = *0x200000e0;
            
            if (*0x200000e0 == 1 || *0x200000e0 == 2 || result == 3)
            {
                result = 0;
                *0x200000e0 = 0;
            }
        }
    }
    else
    {
        if (*0x200000e2 < 0xea60)
            *0x200000e2 += 1;
        
        result = *0x200000e2;
        
        if (result > 5)
        {
            result = *0x200000e0;
            
            if (!result)
            {
                *0x200000e0 = 1;
                *0x200000e4 = 0;
                result = 0x1ef;
                *0x2000012a = 0x1ef;
            }
        }
    }
    
    return result;
}

uint32_t sub_6958()
{
    uint32_t result = *0x200000ee;
    
    if (result == 1)
    {
        uint32_t r0_2 = *0x200000f0 + 1;
        *0x200000f0 = r0_2;
        
        if (r0_2 < 0x32)
        {
            if (*0x200000ec >= 5)
            {
                *0x200000f0 = 0;
                *0x200000ee = 0;
                return sub_4048();
            }
            
            result = *0x200000ec;
            
            if (result >= 1)
            {
                result = *0x20000006;
                
                if (result == 4)
                    return sub_7dd8();
            }
        }
        else
        {
            if (*0x40010810 & 0x80)
            {
                *0x200000f0 = 0;
                *0x200000ee = 0;
                return sub_4048();
            }
            
            result = 0;
            *0x200000f0 = 0;
            *0x200000ee = 0;
            *0x200000ec = 0;
        }
    }
    
    return result;
}

//Main Loop?

int32_t sub_69e0()
{
    int32_t r5;
    int32_t var_c = r5;
    *0x2000001a &= 0xfd;
    sub_2104();
    sub_4d70();
    sub_1cd4();
    sub_2f0c();
    
    while (true)
    {
        sub_1f80();
        *0x20000000 = 0;
        *0x2000001b &= 0xfd;
        *0x2000001c &= 0xdf;
        *0x200000c9 = 0;
        *0x2000001c = (*0x2000001c >> 1 << 1) + 1;
        sub_12fc();
        sub_41a0(1);
        
        if (*0x20000000 != 4)
            break;
        
        sub_6094();
    }
    
    sub_3562(0x40000c00);
    sub_46bc(0x301d, 1);
    sub_4120();
    sub_4bf8();
    int32_t r0_11;
    int32_t r1;
    int32_t r2;
    r0_11 = sub_41e4(0x2dc6c00, 0, r5);
    sub_14ac(r0_11, r1, r2);
    *0x2000001c = *0x2000001c >> 1 << 1;
    sub_5f20();
    sub_5754();
    *0x2000001a = (*0x2000001a & 0xfd) + 2;
    sub_4188();
    sub_2db8(0x40010c00, 0x10);
    sub_2dbc(0x40010800, 0x1000);
    sub_e70();
    sub_4a78();
    sub_3440();
    *0x2000012a = 0;
    sub_7a7c();
    sub_2e58();
    
    if (*0x20000006)
        *0x2000001c = (*0x2000001c & 0xdf) + 0x20;
    else
    {
        bool cond:0_1 = !(*0x40010810 & 8);
        
        while (!cond:0_1)
        {
            int32_t r0_22 = *0x40010810 & 8;
            cond:0_1 = !r0_22;
            
            if (r0_22)
            {
                bool cond:1_1 = *0x20000019 << 0x1e >> 0x1f;
                
                while (!cond:1_1)
                {
                    uint32_t r0_28 = *0x2000001a << 0x1b >> 0x1f;
                    cond:1_1 = r0_28;
                    
                    if (!r0_28)
                    {
                        *0x2000001a = (*0x2000001a & 0xef) + 0x10;
                        uint32_t r0_34 = *0x20000019 << 0x1c >> 0x1f;
                        cond:1_1 = r0_34;
                        
                        if (!r0_34)
                        {
                            uint32_t r0_37 = *0x20000019 << 0x1b >> 0x1f;
                            cond:1_1 = r0_37;
                            
                            if (!r0_37)
                            {
                                *0x20000019 = (*0x20000019 & 0xef) + 0x10;
                                sub_5b0c(sub_1480());
                                *0x2000005a = 0;
                                uint32_t r0_42 = *0x20000150;
                                
                                if (r0_42)
                                {
                                    *0x20000110 = sub_3844();
                                    *0x20000114 = sub_39b0();
                                    uint32_t r0_61;
                                    int32_t r1_5;
                                    r0_61 = sub_698(*0x20000110);
                                    uint32_t r0_63;
                                    int32_t r1_6;
                                    
                                    if (r0_42 > 0)
                                        r0_63 = sub_698(*0x20000114);
                                    
                                    if (r0_42 > 0 && r0_42 > 0)
                                    {
                                        uint32_t r0_65;
                                        int32_t r1_7;
                                        r0_65 = sub_698(*0x20000110);
                                        uint32_t r0_67;
                                        int32_t r1_8;
                                        
                                        if (r0_42 > 0)
                                            r0_67 = sub_698(*0x20000114);
                                        
                                        if (r0_42 > 0 && r0_42 > 0)
                                        {
                                            sub_4d30();
                                            *0x2000001d = (*0x2000001d & 0xfd) + 2;
                                            *0x20000128 = 0;
                                            *0x20000124 = 0;
                                            sub_1330();
                                            sub_133c();
                                            *0x2000001b &= 0xfd;
                                            *0x20000019 = (*0x20000019 & 0xdf) + 0x20;
                                            *0x20000006 = 2;
                                        }
                                        else
                                            sub_1518();
                                    }
                                    else
                                        sub_1518();
                                }
                                else
                                {
                                    *0x20000110 = sub_3844();
                                    *0x20000114 = sub_39b0();
                                    uint32_t r0_46;
                                    int32_t r1_1;
                                    r0_46 = sub_698(*0x20000110);
                                    uint32_t r0_48;
                                    int32_t r1_2;
                                    
                                    if (r0_42 > 0)
                                        r0_48 = sub_698(*0x20000114);
                                    
                                    if (r0_42 > 0 && r0_42 > 0)
                                    {
                                        uint32_t r0_50;
                                        int32_t r1_3;
                                        r0_50 = sub_698(*0x20000110);
                                        uint32_t r0_52;
                                        int32_t r1_4;
                                        
                                        if (r0_42 > 0)
                                            r0_52 = sub_698(*0x20000114);
                                        
                                        if (r0_42 > 0 && r0_42 > 0)
                                        {
                                            sub_4d30();
                                            *0x20000128 = 0;
                                            *0x20000124 = 0;
                                            sub_1330();
                                            sub_133c();
                                            *0x2000001b &= 0xfd;
                                            *0x20000019 = (*0x20000019 & 0xdf) + 0x20;
                                            *0x20000006 = 2;
                                        }
                                        else
                                            sub_1518();
                                    }
                                    else
                                        sub_1518();
                                }
                                
                                break;
                            }
                        }
                    }
                }
                
                break;
            }
        }
    }
    
    if (!*0x20000006)
        *0x20000006 = 1;
    
    char result = (*0x2000001a & 0xfd) + 2;
    *0x2000001a = result;
    return result;
}

uint32_t sub_6d38()
{
    uint32_t result = *0x20000066;
    
    if (result < 0xfffa)
    {
        result = *0x20000066 + 1;
        *0x20000066 = result;
    }
    
    return result;
}

uint32_t sub_6d58()
{
    *0x2000012a = 0;
    *0x20000006 = 0;
    sub_2f64();
    return sub_3f84();
}

uint32_t sub_6d78()
{
    uint32_t result = *0x20000008;
    
    if (result == 1)
    {
        *0x20000008 = 0;
        
        if (*0x20000006 && *0x20000006 != 0xe && *0x20000006 != 0xb && *0x20000006 != 8
                && *0x20000006 != 9 && *0x20000006 != 0xa)
            result = *0x20000006;
        
        if (!*0x20000006 || *0x20000006 == 0xe || *0x20000006 == 0xb || *0x20000006 == 8
                || *0x20000006 == 9 || *0x20000006 == 0xa || result == 0xb)
            return sub_3b60();
    }
    
    return result;
}

void sub_6dd0() __noreturn
{
    sub_5f20();
    sub_5754();
    sub_4188();
    sub_2db8(0x40010c00, 0x10);
    sub_2dbc(0x40010800, 0x1000);
    sub_6094();
    sub_65e0(0x1f4);
    sub_3280();
    sub_6f94();
    sub_e70();
    sub_6094();
    sub_65e0(0x1f4);
    sub_6094();
    sub_4a78();
    sub_3440();
    sub_290c();
    sub_48a4(0x200000fc, 0, 0xc);
    
    if (sub_12dc(0x200000fc, 0x8007e30, 8))
    {
        __dsb_SY();
        *0xe000ed0c = 0x5fa0004;
        __dsb_SY();
        
        while (true)
            /* nop */
        
        return;
    }
    
    int32_t r0_1;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    r0_1 = sub_f64();
    sub_4814(r0_1, r1, r2, r3);
    *0x2000001c = *0x2000001c >> 1 << 1;
    *0x20000150 = 1;
    *0x200000d8 = 1;
    sub_2f64();
    *0x2000012a = 0;
    sub_3f84();
    *0x2000001a = (*0x2000001a & 0xfd) + 2;
    
    while (true)
    {
        if (!*0x200000c9 && *0x2000012a > 0x1f4 && *0x40010c10 & 2 && !sub_2e08()
            && (!*0x20000006 || *0x20000006 == 4))
        {
            *0x2000012a = 0;
            
            if (*0x40010c10 & 2)
                sub_69e0();
        }
        
        sub_59d0();
        sub_5440();
        sub_6d78();
        sub_1354();
        sub_7024();
        
        if (!*0x200000c9)
        {
            if (!(*0x40010c10 & 2))
            {
                if (*0x2000001b << 0x1a >> 0x1f && (!*0x20000006 || *0x20000006 == 4))
                    sub_1360();
            }
            else if (!*0x20000006 || *0x20000006 == 4)
                sub_1360();
        }
        
        sub_4884();
        sub_7aac();
        sub_6250();
        sub_6094();
    }
}

uint32_t sub_6f84()
{
    sub_705c();
    sub_4328();
    return sub_3d94();
}

int32_t sub_6f94()
{
    sub_7308(4);
    int32_t result;
    
    if (*0x200000f3 == 0x33 && *0x200000f4 == 0x30 && *0x200000f5 == 0x25)
    {
        *0x200000f3 = 0xff;
        *0x200000f4 = 0xff;
        *0x200000f5 = 0xff;
        sub_7308(0xd3);
        
        if (*0x200000f3 != 0x33 || *0x200000f4 != 0x30 || *0x200000f5 != 0x25)
        {
            result = 1;
            *0x200000f2 = 1;
        }
        else
        {
            result = 0;
            *0x200000f2 = 0;
        }
    }
    else if (*0x200000f3 != 0x7c || *0x200000f4 != 0x89 || *0x200000f5 != 0xf0)
    {
        result = 0;
        *0x200000f2 = 0;
    }
    else
    {
        result = 2;
        *0x200000f2 = 2;
    }
    
    return result;
}

uint32_t sub_7024()
{
    uint32_t result = *0x2000001a << 0x1e >> 0x1f;
    
    if (result)
    {
        result = *0x20000050 + 1;
        *0x20000050 = result;
        
        if (result > 0x7530)
        {
            *0x20000050 = 0;
            sub_3d5c();
            /* no return */
        }
    }
    
    return result;
}

uint32_t sub_705c()
{
    uint32_t r0_2 = *0x20000014 + 1;
    *0x20000014 = r0_2;
    
    if (r0_2 >= 0x1c2)
    {
        uint32_t r0_3 = *0x200000c9;
        
        if (!r0_3)
        {
            *0x20000014 = r0_3;
            
            if (*0x40010c10 & 2)
                *0x20000011 = 0;
            else if (*0x20000011)
            {
                sub_63fc();
                sub_1de4(*0x20000012);
            }
            else
            {
                *0x20000011 = 1;
                *0x20000012 = *0x20000120;
                sub_1de4(*0x20000012);
            }
        }
    }
    
    if (!*0x200000c9 && *0x40010c10 & 2)
    {
        if (*0x20000120 > 1)
            sub_1de4(*0x20000120);
        else
        {
            uint32_t r0_16 = *0x20000016 + 1;
            *0x20000016 = r0_16;
            
            if (r0_16 > 0x1f4)
            {
                *0x20000016 = 0;
                
                if (*0x2000001c << 0x1e >> 0x1f)
                {
                    *0x2000001c &= 0xfd;
                    sub_1de4(0);
                }
                else
                {
                    *0x2000001c = (*0x2000001c & 0xfd) + 2;
                    sub_1de4(1);
                }
            }
        }
    }
    
    uint32_t result = *0x200000c9;
    
    if (!result)
    {
        int32_t r0_27 = *0x40010c10 & 2;
        
        if (r0_27)
        {
            if (*0x2000000c < 0xc8)
                *0x2000000c += 1;
            
            result = *0x2000000c;
            
            if (result > 0x64)
            {
                *0x20000011 = 0;
                *0x2000001b &= 0xdf;
                return sub_402c();
            }
        }
        else
        {
            *0x2000000c = r0_27;
            result = *0x2000001b << 0x1a >> 0x1f;
            
            if (!result)
                return sub_3f84();
        }
    }
    
    return result;
}

uint32_t sub_71b8()
{
    uint32_t result;
    
    if (*0x40011010 & 0x8000)
    {
        if (*0x200000ea < 0xea60)
            *0x200000ea += 1;
        
        result = *0x200000ea;
        
        if (result > 2)
        {
            *0x200000ea = 0;
            *0x200000e8 = 0;
            
            if (*0x200000e6 != 1 && *0x200000e6 != 2)
                result = *0x200000e6;
            
            if (*0x200000e6 == 1 || *0x200000e6 == 2 || result == 3)
            {
                *0x200000e6 = 0;
                *0x20000150 = 1;
                return sub_2f64();
            }
        }
    }
    else
    {
        if (*0x200000e8 < 0xea60)
            *0x200000e8 += 1;
        
        result = *0x200000e8;
        
        if (result > 5)
        {
            result = *0x200000e6;
            
            if (!result)
            {
                *0x200000e6 = 1;
                *0x200000ea = 0;
                *0x20000150 = 0;
                return sub_2f64();
            }
        }
    }
    
    return result;
}

uint32_t sub_7260()
{
    uint32_t result;
    
    if (*0x2000000e < 2 || !(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x2000000e;
        
        if (result > 4)
        {
            *0x2000012a = 0;
            *0x20000006 = 0;
            sub_5338();
            result = 0;
            *0x2000000e = 0;
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        sub_1518();
        sub_6470();
        
        if (*0x2000001b << 0x1e >> 0x1f)
        {
            sub_2e1c();
            sub_3790();
            sub_2f64();
            *0x2000001b &= 0xfd;
        }
        
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

int32_t sub_72e8()
{
    *0x2000000e = 0;
    char result = (*0x20000019 & 0xbf) + 0x40;
    *0x20000019 = result;
    return result;
}

int32_t sub_7308(int32_t arg1)
{
    sub_5b50(arg1);
    sub_7718();
    sub_2db8(0x40010c00, 8);
    sub_2dbc(0x40010c00, 8);
    *0x200000f3 = sub_4ba8();
    *0x200000f4 = sub_4ba8();
    *0x200000f5 = sub_4ba8();
    sub_2dbc(0x40010800, 0x8000);
    return sub_7740();
}

int32_t sub_736c(int32_t arg1)
{
    sub_7690();
    sub_2db8(0x40010800, 0x100);
    sub_74e8(0x20);
    sub_74e8((0xff0000 & arg1) >> 0x10);
    sub_74e8((0xff00 & arg1) >> 8);
    sub_74e8(arg1);
    sub_2dbc(0x40010800, 0x100);
    return sub_7530();
}

int32_t sub_73b8()
{
    sub_4a5c(0x40014400, 0);
    sub_4acc(0x40014400);
    sub_46bc(0x400, 0);
    int32_t var_1c = 0x200;
    int32_t var_18 = 0;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x400;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x800;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x100;
    return sub_2ba0(0x40010800, &var_1c);
}

int32_t sub_741c()
{
    int32_t var_1c;
    sub_2d88(&var_1c);
    sub_46bc(4, 1);
    sub_46bc(0x401, 1);
    var_1c = 0x200;
    int32_t var_18 = 2;
    int32_t var_8 = 0;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x800;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x400;
    int32_t var_18_1 = 2;
    sub_2ba0(0x40010800, &var_1c);
    var_1c = 0x100;
    int32_t var_18_2 = 1;
    return sub_2ba0(0x40010800, &var_1c);
}

int32_t sub_748c(char* arg1, int32_t arg2, uint32_t arg3)
{
    char* r5 = arg1;
    uint32_t r6 = arg3;
    sub_2db8(0x40010800, 0x100);
    sub_74e8(3);
    sub_74e8((0xff0000 & arg2) >> 0x10);
    sub_74e8((0xff00 & arg2) >> 8);
    sub_74e8(arg2);
    
    while (true)
    {
        uint32_t r0_4 = r6;
        r6 = r6 - 1;
        
        if (!r0_4)
            break;
        
        *r5 = sub_74e8(0xa5);
        r5 = &r5[1];
    }
    
    sub_2dbc(0x40010800, 0x100);
    return 0x40010800;
}

uint32_t sub_74e8(int16_t arg1)
{
    *0x20000058 = 0;
    int32_t i;
    
    do
        i = sub_4b1c(0x40014400, 2);
     while (!i);
    sub_4b62(0x40014400, arg1);
    *0x20000058 = 0;
    int32_t i_1;
    
    do
        i_1 = sub_4b1c(0x40014400, 1);
     while (!i_1);
    return sub_4b5c(0x40014400);
}

int32_t sub_7530()
{
    sub_2db8(0x40010800, 0x100);
    sub_74e8(5);
    uint32_t i;
    
    do
        i = sub_74e8(0xa5) << 0x1f >> 0x1f;
     while (i);
    sub_2dbc(0x40010800, 0x100);
    return 0x40010800;
}

uint32_t sub_7568(char* arg1, int32_t arg2, uint32_t arg3)
{
    char* r7 = arg1;
    int32_t r6 = arg2;
    int32_t var_1c = 0;
    int32_t var_20 = 0;
    int32_t var_24 = 0;
    int32_t var_28 = 0;
    uint32_t r0 = r6;
    uint32_t r5 = 0 - r0;
    uint32_t var_1c_1 = ((arg3 >> 0x1f >> 0x18) + arg3) << 0x10 >> 0x18;
    uint32_t r1_11 = arg3 - (((arg3 >> 0x1f >> 0x18) + arg3) >> 8 << 8);
    
    if (!r0)
    {
        if (var_1c_1)
        {
            while (true)
            {
                uint32_t r0_9 = var_1c_1;
                var_1c_1 = r0_9 - 1;
                
                if (!r0_9)
                    break;
                
                sub_76b4(r7, r6, 0x100);
                r6 += 0x100;
                r7 = &r7[0x100];
            }
            
            return sub_76b4(r7, r6, r1_11);
        }
        
        return sub_76b4(r7, r6, arg3);
    }
    
    if (!var_1c_1)
    {
        if (r1_11 <= r5)
            return sub_76b4(r7, r6, arg3);
        
        sub_76b4(r7, r6, r5);
        return sub_76b4(&r7[r5], r6 + r5, r1_11 - r5);
    }
    
    uint32_t r4_1 = arg3 - r5;
    uint32_t var_1c_2 = ((r4_1 >> 0x1f >> 0x18) + r4_1) << 0x10 >> 0x18;
    uint32_t result = r4_1 - (((r4_1 >> 0x1f >> 0x18) + r4_1) >> 8 << 8);
    sub_76b4(r7, r6, r5);
    int32_t r6_3 = r6 + r5;
    void* r7_3 = &r7[r5];
    
    while (true)
    {
        uint32_t r0_23 = var_1c_2;
        var_1c_2 = r0_23 - 1;
        
        if (!r0_23)
            break;
        
        sub_76b4(r7_3, r6_3, 0x100);
        r6_3 += 0x100;
        r7_3 += 0x100;
    }
    
    if (!result)
        return result;
    
    return sub_76b4(r7_3, r6_3, result);
}

int32_t sub_7690()
{
    sub_2db8(0x40010800, 0x100);
    sub_74e8(6);
    sub_2dbc(0x40010800, 0x100);
    return 0x40010800;
}

int32_t sub_76b4(char* arg1, int32_t arg2, uint32_t arg3)
{
    char* r5 = arg1;
    uint32_t r6 = arg3;
    sub_7690();
    sub_2db8(0x40010800, 0x100);
    sub_74e8(2);
    sub_74e8((0xff0000 & arg2) >> 0x10);
    sub_74e8((0xff00 & arg2) >> 8);
    sub_74e8(arg2);
    
    while (true)
    {
        uint32_t r0_4 = r6;
        r6 = r6 - 1;
        
        if (!r0_4)
            break;
        
        sub_74e8(*r5);
        r5 = &r5[1];
    }
    
    sub_2dbc(0x40010800, 0x100);
    return sub_7530();
}

int32_t sub_7718()
{
    sub_2db8(0x40010c00, 0x20);
    int32_t var_1c = 0x20;
    int32_t var_10 = 0;
    int32_t var_18 = 0;
    return sub_2ba0(0x40010c00, &var_1c);
}

int32_t sub_7740()
{
    sub_2db8(0x40010c00, 0x20);
    int32_t var_1c = 0x20;
    int32_t var_10 = 0;
    int32_t var_18 = 1;
    return sub_2ba0(0x40010c00, &var_1c);
}

void sub_7768(char arg1)
{
    *0x200000d6 = arg1;
}

void sub_7774(char arg1)
{
    *0x200000d7 = arg1;
}

void sub_7780(int32_t arg1)
{
    *0x20000019 = (*0x20000019 & 0x7f) | (arg1 << 7 & 0x80);
}

int32_t sub_779c()
{
    int32_t result;
    
    if (*0x20000130 <= 0x5a550)
    {
        result = 0;
        *0x20000152 = 0;
    }
    else
    {
        result = 1;
        *0x20000152 = 1;
    }
    
    return result;
}

uint32_t sub_77c4()
{
    uint32_t result;
    
    if (*0x40010810 & 0x80)
    {
        result = *0x40010810 & 0x80;
        
        if (result)
        {
            if (*0x200000de < 0xea60)
                *0x200000de += 1;
            
            result = *0x200000de;
            
            if (result > 2)
            {
                *0x200000de = 0;
                *0x200000dc = 0;
                *0x2000001c &= 0xfb;
                *0x2000001d &= 0xfb;
                *0x2000001c &= 0x7f;
                *0x2000001d = *0x2000001d >> 1 << 1;
                
                if (*0x200000da != 1 && *0x200000da != 2 && *0x200000da != 3)
                    result = *0x200000da;
                
                if (*0x200000da == 1 || *0x200000da == 2 || *0x200000da == 3 || result == 4)
                {
                    result = 0;
                    *0x200000da = 0;
                }
            }
        }
    }
    else
    {
        result = *0x200000c9;
        
        if (!result)
        {
            if (*0x200000dc < 0xea60)
                *0x200000dc += 1;
            
            if (*0x200000dc > 1 && !*0x200000da)
            {
                *0x200000da = 1;
                result = 0;
                *0x2000012a = 0;
            }
            else if (*0x200000dc > 2 && *0x200000da == 1)
            {
                *0x200000ec += 1;
                *0x200000da = 2;
                *0x200000ee = 1;
                result = 0;
                *0x200000f0 = 0;
                *0x200000de = 0;
            }
            else if (*0x200000dc <= 0x1388)
            {
                result = *0x200000dc;
                
                if (result > 0x1f4)
                {
                    *0x200000da = 3;
                    result = 0;
                    *0x2000012a = 0;
                }
            }
            else
            {
                result = 4;
                *0x200000da = 4;
            }
        }
    }
    
    return result;
}

uint32_t sub_791c()
{
    if (*0x20000019 << 0x1a >> 0x1f)
    {
        *0x2000012a = 0;
        
        if (*0x20000128 != *0x20000124)
            *0x20000128 = *0x20000124;
        
        sub_4f44();
        
        if (*0x2000001b << 0x1d >> 0x1f)
        {
            sub_5e2c();
            sub_3f84();
        }
        else
            sub_5e2c();
    }
    
    uint32_t result;
    
    if (*0x40010810 & 8)
        result = *0x20000019 << 0x1a >> 0x1f;
    
    if (!(*0x40010810 & 8) || !result)
    {
        if (*0x20000019 << 0x1a >> 0x1f)
        {
            *0x20000019 &= 0xdf;
            sub_1518();
            sub_6470();
        }
        
        *0x2000012a = 0;
        *0x20000019 = (*0x20000019 & 0xbf) + 0x40;
        *0x2000000e = 0;
        *0x20000006 = 0xe;
        *0x20000126 = *0x20000124;
        
        if (*0x20000150)
        {
            int32_t r0_26;
            int32_t r1_4;
            r0_26 = sub_618(*0x20000126);
            int32_t r0_27;
            int32_t r1_5;
            r0_27 = sub_458(r0_26, r1_4, 0xeb851eb8, 0x3ffab851);
            result = sub_65c(r0_27, r1_5) + *0x20000144;
            *0x20000144 = result;
        }
        else
        {
            *0x20000144 += *0x20000126;
            result = *0x20000158 + *0x20000126;
            *0x20000158 = result;
        }
    }
    
    return result;
}

uint32_t sub_7a2c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    uint32_t result;
    
    if (*0x20000065 != 0xd)
    {
        result = *0x20000019 << 0x1a >> 0x1f;
        
        if (result)
        {
            *0x2000012a = 0;
            return sub_62d0();
        }
    }
    else
    {
        *0x20000006 = 3;
        result = (*0x2000001b & 0xfffffffb) + 4;
        *0x2000001b = result;
    }
    
    return result;
}

int32_t sub_7a7c()
{
    *0x20000066 = 0;
    *0x20000065 = 0;
    return 0;
}

void sub_7a90(int32_t arg1)
{
    *0x2000001c = (*0x2000001c & 0xf7) | (arg1 << 3 & 8);
}

uint32_t sub_7aac()
{
    uint32_t result = *0x2000001c << 0x1c >> 0x1f;
    
    if (!result)
        return result;
    
    *0x2000001c &= 0xf7;
    
    if (*0x200000da != 2)
    {
        if (*0x200000da != 3)
            /* tailcall */
            return sub_7c0c(*0x200000da);
        
        uint32_t r0_37 = *0x2000001d << 0x1f >> 0x1f;
        
        if (r0_37)
            /* tailcall */
            return sub_7c64(r0_37);
        
        *0x2000001d = (*0x2000001d >> 1 << 1) + 1;
        uint32_t r0_43 = *0x20000006;
        
        if (r0_43)
            /* tailcall */
            return sub_7c64(r0_43);
        
        uint32_t r0_47 = *0x2000001b << 0x19 >> 0x1f;
        
        if (r0_47)
            /* tailcall */
            return sub_7c64(r0_47);
        
        if (*0x200000d8 != 1)
            /* tailcall */
            return sub_7bf6();
        
        /* tailcall */
        return sub_7bf2(0, 0x200000d8);
    }
    
    *0x2000012a = 0;
    
    if (!(*0x2000001c >> 7))
    {
        *0x2000001c = (*0x2000001c & 0x7f) - 0x80;
        
        if (*0x20000006)
        {
            if (*0x20000006 == 0xe && !(*0x2000001c << 0x1d >> 0x1f))
            {
                *0x2000001c = (*0x2000001c & 0xfb) + 4;
                
                if (*0x20000150)
                    *0x20000150 = 0;
                else
                    *0x20000150 = 1;
                
                sub_30a8(0, 0, 0x80, 0xa0, 0);
                *0x2000012a = 0;
                *0x20000124 = 0;
                *0x20000006 = 0;
                sub_5338();
                sub_2f64();
            }
        }
        else if (!(*0x2000001c << 0x1d >> 0x1f))
        {
            *0x2000001c = (*0x2000001c & 0xfb) + 4;
            
            if (*0x20000150)
                *0x20000150 = 0;
            else
                *0x20000150 = 1;
            
            sub_2f64();
        }
    }
    
    if (*0x20000006)
    {
        uint32_t r0_27 = *0x20000006;
        
        if (r0_27 != 4)
            /* tailcall */
            return sub_7c64(r0_27);
        
        int32_t r0_30 = *0x40010c10 & 2;
        
        if (r0_30)
            /* tailcall */
            return sub_7c64(r0_30);
    }
    
    uint32_t r0_32 = sub_3f84();
    *0x2000012a = 0;
    /* tailcall */
    return sub_7c64(r0_32);
}

int32_t sub_7bf2(char arg1, char* arg2)
{
    *arg2 = arg1;
    /* tailcall */
    return sub_7bfc();
}

int32_t sub_7bf6()
{
    *0x200000d8 = 1;
    /* tailcall */
    return sub_7bfc();
}

int32_t sub_7bfc()
{
    *0x20000006 = 0xd;
    /* tailcall */
    return sub_7c64(sub_72e8());
}

int32_t sub_7c0c(int32_t arg1)
{
    if (arg1 == 4)
    {
        uint32_t r0_2 = *0x2000001d << 0x1d >> 0x1f;
        
        if (r0_2)
            /* tailcall */
            return sub_7c64(r0_2);
        
        *0x2000001d = (*0x2000001d & 0xfb) + 4;
        *0x20000006 = 0xc;
        /* tailcall */
        return sub_7c64(sub_72e8());
    }
    
    uint32_t r0_9 = *0x200000da;
    
    if (r0_9 != 1)
        /* tailcall */
        return sub_7c64(r0_9);
    
    if (*0x20000006)
    {
        uint32_t r0_12 = *0x20000006;
        
        if (r0_12 != 4)
            /* tailcall */
            return sub_7c64(r0_12);
        
        int32_t r0_15 = *0x40010c10 & 2;
        
        if (r0_15)
            /* tailcall */
            return sub_7c64(r0_15);
    }
    
    uint32_t r0_17 = sub_3f84();
    *0x2000012a = 0;
    /* tailcall */
    return sub_7c64(r0_17);
}

uint32_t sub_7c64(int32_t arg1)
{
    sub_71b8();
    return sub_68bc();
}

int32_t sub_7c72()
{
    /* tailcall */
    return sub_7c94();
}

int32_t sub_7c94()
{
    int16_t result = *0x2000000e + 1;
    *0x2000000e = result;
    return result;
}

uint32_t sub_7ca2()
{
    int32_t r4;
    int32_t var_8 = r4;
    uint32_t r0_2 = *0x2000001c << 0x1a >> 0x1f;
    
    if (!r0_2)
        return sub_541c();
    
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    sub_29c0(r0_2, entry_r1, entry_r2, entry_r3, r4);
    uint32_t result = *0x20000065;
    
    if (result == 0xd)
    {
        result = *0x2000001c & 0xffffffdf;
        *0x2000001c = result;
    }
    
    return result;
}

uint32_t sub_7cd2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t var_8 = arg4;
    
    if (*0x20000019 << 0x19 >> 0x1f)
    {
        *0x20000019 &= 0xbf;
        *0x2000000e = 0;
        *0x2000012a = 0;
        return sub_3790();
    }
    
    uint32_t result = *0x2000000e;
    
    if (result >= 2)
    {
        result = *0x20000019 << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000012a = 0xc8;
            *0x20000011 = 0;
            sub_2104();
            sub_3790();
            result = 4;
            *0x20000006 = 4;
        }
    }
    
    return result;
}

uint32_t sub_7d36()
{
    int32_t r4;
    int32_t var_8 = r4;
    uint32_t result;
    
    if (!(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x20000019 << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000012a = result;
            *0x20000019 &= 0xfd;
            *0x20000011 = 0;
            *0x20000006 = 0;
            *0x20000150 = 1;
            *0x200000d8 = 1;
            sub_3790();
            sub_2f64();
            sub_5338();
            return sub_3f84();
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        *0x2000000e = 0;
        sub_5338();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

int32_t sub_7dbc()
{
    /* tailcall */
    return sub_7dd8();
}

uint32_t sub_7dd8()
{
    uint32_t result = *0x2000001b << 0x19 >> 0x1f;
    
    if (result)
        return result;
    
    *0x20000006 = 6;
    *0x200000ec = 0;
    return sub_72e8();
}

int32_t sub_7dfc(int32_t arg1 @ r5)
{
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    /* tailcall */
    return sub_7e04(0, entry_r1, entry_r2, entry_r3);
}

uint32_t sub_7e04(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    int32_t var_8 = r4;
    sub_29c0(arg1, arg2, arg3, arg4, r4);
    uint32_t result = *0x20000065;
    
    if (result != 0xd)
        return result;
    
    *0x2000012a = 0;
    *0x20000006 = 7;
    return sub_72e8();
}

int32_t sub_7e24(int32_t arg1 @ r4)
{
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    /* tailcall */
    return sub_7e34(0, entry_r1, entry_r2, entry_r3, arg1);
}

int32_t sub_7e34(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_7e82(int32_t arg1)
{
    int32_t entry_r2;
    int32_t entry_r3;
    int32_t r4;
    /* tailcall */
    return sub_7e8a(arg1, arg1, entry_r2, entry_r3, r4);
}

int32_t sub_7e8a(int32_t arg1, int32_t arg2, int32_t arg3, char* arg4, void* arg5 @ r4)
{
    *(arg4 + arg5) = arg1;
    uint32_t r0 = *(arg5 + arg1);
    int32_t r5;
    int32_t r7;
    /* tailcall */
    return sub_7e9c(r0, r0, arg3, arg4, r5, r7);
}

int32_t sub_7e9c(int32_t arg1, int32_t arg2, void* arg3, void* arg4, void* arg5 @ r5, void* arg6 @ r7, int32_t arg7)
{
    *(arg5 + 0x30) = arg1;
    void* r0 = *(arg4 + 0x20);
    *(arg6 + 4) = r0;
    *(arg3 + 0x32) = *(*(r0 + 1) + 0x1d);
    *(arg4 + 0x2a);
    /* undefined */
}

void sub_7f02() __noreturn
{
    breakpoint();
}

int32_t sub_7f12(int32_t arg1, int32_t arg2, int32_t arg3, char* arg4, int32_t* arg5 @ r4)
{
    int32_t r5 = *arg5;
    int32_t r7 = arg5[1];
    bool c;
    
    if (!c)
        /* jump -> arg1 */
    
    bool v;
    
    if (arg4 < 0 != v)
        /* tailcall */
        return sub_82a4();
    
    *(arg4 + &arg5[2]) = arg1;
    uint32_t r0_2 = *(&arg5[2] + arg1);
    /* tailcall */
    return sub_7e9c(r0_2, r0_2, arg3, arg4, r5, r7);
}

int32_t sub_7f36()
{
    /* undefined */
}

int32_t sub_7f42()
{
    /* undefined */
}

int32_t sub_7f4c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, void* arg5 @ r4, int32_t arg6 @ r7)
{
    /* unimplemented  {ldc2l p0, c0, [r8, #4]} */
    *(arg5 + 0x14) = arg4;
    *(arg5 + 0x5c) = arg4;
    /* tailcall */
    return j_sub_7bf2(0, arg2);
}

int32_t j_sub_7bf2(char arg1, char* arg2)
{
    /* tailcall */
    return sub_7bf2(arg1, arg2);
}

int32_t j_sub_7bf6()
{
    /* tailcall */
    return sub_7bf6();
}

int32_t j_sub_7c0c(int32_t arg1)
{
    /* tailcall */
    return sub_7c0c(arg1);
}

int32_t j_sub_7c72()
{
    /* tailcall */
    return sub_7c72();
}

int32_t sub_82a4()
{
    /* tailcall */
    return sub_7c94();
}

uint32_t j_sub_7ca2()
{
    /* tailcall */
    return sub_7ca2();
}

uint32_t j_sub_7cd2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    /* tailcall */
    return sub_7cd2(arg1, arg2, arg3, arg4);
}

uint32_t j_sub_7d36()
{
    /* tailcall */
    return sub_7d36();
}

uint32_t sub_8374()
{
    uint32_t result;
    
    if (!(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x20000019 << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000012a = result;
            *0x20000019 &= 0xfd;
            *0x20000011 = 0;
            *0x20000006 = 0;
            *0x20000150 = 1;
            *0x200000d8 = 1;
            sub_3790();
            sub_2f64();
            sub_5338();
            return sub_3f84();
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        *0x2000000e = 0;
        sub_5338();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

uint32_t sub_8376()
{
    uint32_t result;
    
    if (!(*0x20000019 << 0x19 >> 0x1f))
    {
        result = *0x20000019 << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000012a = result;
            *0x20000019 &= 0xfd;
            *0x20000011 = 0;
            *0x20000006 = 0;
            *0x20000150 = 1;
            *0x200000d8 = 1;
            sub_3790();
            sub_2f64();
            sub_5338();
            return sub_3f84();
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        *0x2000000e = 0;
        sub_5338();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

uint32_t sub_8384(int32_t arg1)
{
    uint32_t result;
    bool z;
    
    if (z)
    {
        result = *0x20000019 << 0x19 >> 0x1f;
        
        if (!result)
        {
            *0x2000012a = result;
            *0x20000019 &= 0xfd;
            *0x20000011 = 0;
            *0x20000006 = 0;
            *0x20000150 = 1;
            *0x200000d8 = 1;
            sub_3790();
            sub_2f64();
            sub_5338();
            return sub_3f84();
        }
    }
    else
    {
        *0x20000019 &= 0xbf;
        *0x2000000e = 0;
        sub_5338();
        result = 0;
        *0x2000012a = 0;
    }
    
    return result;
}

uint32_t sub_83c0(int32_t arg1)
{
    *0x20000011 = 0;
    *0x20000006 = 0;
    *0x20000150 = 1;
    *0x200000d8 = 1;
    sub_3790();
    sub_2f64();
    sub_5338();
    return sub_3f84();
}

int32_t sub_83cc(int32_t arg1 @ r10)
{
    /* jump -> *(arg1 + 0x7fd) */
}

int32_t j_sub_7dbc()
{
    /* tailcall */
    return sub_7dbc();
}

int32_t sub_83ee()
{
    /* tailcall */
    return sub_7dd8();
}

int32_t sub_83fa()
{
    /* tailcall */
    return sub_7dd8();
}

int32_t sub_83fc()
{
    /* tailcall */
    return sub_7dd8();
}

int32_t sub_8402(int32_t arg1, int32_t arg2)
{
    if (arg1)
        return arg1;
    
    *0x20000006 = 6;
    *0x200000ec = 0;
    return sub_72e8();
}

void sub_8410(int32_t arg1)
{
    bool z;
    
    if (!z)
        return;
    
    *0x20000006 = 6;
    *0x200000ec = 0;
    sub_72e8();
}

int32_t sub_8412(char arg1, char* arg2, int32_t arg3)
{
    *arg2 = arg1;
    *0x200000ec = 0;
    return sub_72e8();
}

int32_t sub_8422()
{
    int32_t r5;
    /* tailcall */
    return j_sub_7dfc(r5);
}

int32_t j_sub_7dfc(int32_t arg1 @ r5)
{
    /* tailcall */
    return sub_7dfc(arg1);
}

int32_t sub_842a(int32_t arg1 @ r5)
{
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    /* tailcall */
    return sub_7e04(0, entry_r1, entry_r2, entry_r3);
}

uint32_t sub_8436(int32_t arg1)
{
    uint32_t result = *0x20000065;
    
    if (result != 0xd)
        return result;
    
    *0x2000012a = 0;
    *0x20000006 = 7;
    return sub_72e8();
}

uint32_t sub_8438(char* arg1, int32_t arg2)
{
    uint32_t result = *arg1;
    
    if (result != 0xd)
        return result;
    
    *0x2000012a = 0;
    *0x20000006 = 7;
    return sub_72e8();
}

int32_t sub_8442()
{
    int32_t r4;
    /* tailcall */
    return sub_7e24(r4);
}

int32_t sub_8452()
{
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    int32_t r4;
    /* tailcall */
    return sub_7e34(0, entry_r1, entry_r2, entry_r3, r4);
}

int32_t sub_8454()
{
    int32_t entry_r1;
    int32_t entry_r2;
    int32_t entry_r3;
    int32_t r4;
    /* tailcall */
    return sub_7e34(0, entry_r1, entry_r2, entry_r3, r4);
}

int32_t j_sub_7e34(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_7e34(arg1, arg2, arg3, arg4, arg5);
}

int32_t sub_8464(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_8466(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_8472(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_8474(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_847a(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_847c(int32_t arg1, void* arg2, int32_t, void* arg4, void* arg5 @ r4)
{
    uint32_t r5 = *(arg2 + 0x15);
    *(arg4 + 0x50) = arg2;
    *(arg2 + 0x48) = r5;
    *(arg4 + 8) = r5;
    *(arg5 + 9) = arg2;
    *0x6c = arg2;
    *r5;
    /* jump -> 0 */
}

int32_t sub_848a(int32_t arg1, int32_t arg2, void* arg3 @ r4, int32_t* arg4 @ r5)
{
    *(arg3 + 9) = arg2;
    *0x6c = arg2;
    *arg4;
    /* jump -> 0 */
}

int32_t sub_849a(int32_t arg1)
{
    /* jump -> arg1 >> 0x20 */
}

int32_t sub_84a2(int32_t arg1)
{
    /* tailcall */
    return sub_7e82(arg1);
}

int32_t sub_84b2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t r4;
    /* tailcall */
    return sub_7e8a(arg1, arg2, arg3, arg4, r4);
}

int32_t sub_84b8(int32_t arg1, int32_t arg2, int32_t arg3, char* arg4, void* arg5 @ r4)
{
    *(arg4 + arg5) = arg1;
    uint32_t r0 = *(arg5 + arg1);
    int32_t r5;
    int32_t r7;
    /* tailcall */
    return sub_7e9c(r0, r0, arg3, arg4, r5, r7);
}

int32_t sub_84ba(int32_t arg1)
{
    int32_t entry_r2;
    int32_t entry_r3;
    int32_t r5;
    int32_t r7;
    /* tailcall */
    return sub_7e9c(arg1, arg1, entry_r2, entry_r3, r5, r7);
}

int32_t j_sub_7e9c(int32_t arg1, int32_t arg2, void* arg3, void* arg4, int32_t arg7)
{
    void* arg5;
    void* arg6;
    /* tailcall */
    return sub_7e9c(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

int32_t sub_84ca(void* arg1, int32_t arg2, void* arg3, void* arg4, void* arg5 @ r7, int32_t arg6)
{
    *(arg5 + 4) = arg1;
    *(arg3 + 0x32) = *(*(arg1 + 1) + 0x1d);
    *(arg4 + 0x2a);
    /* undefined */
}

int32_t sub_84dc(void* arg1, int32_t arg2, void* arg3, void* arg4, void* arg5 @ r7, int32_t arg6)
{
    *(arg5 + 4) = arg1;
    *(arg3 + 0x32) = *(*(arg1 + 1) + 0x1d);
    *(arg4 + 0x2a);
    /* undefined */
}

int32_t sub_8502(int32_t arg1)
{
    /* undefined */
}

int32_t sub_851a()
{
    /* undefined */
}

int32_t sub_8528()
{
    /* undefined */
}

void sub_852a() __noreturn
{
    /* tailcall */
    return sub_7f02();
}

int32_t j_sub_7f12(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_7f12(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_7f36()
{
    /* tailcall */
    return sub_7f36();
}

int32_t j_sub_7f42()
{
    /* tailcall */
    return sub_7f42();
}

int32_t j_sub_7f4c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, void* arg5 @ r4, int32_t arg6 @ r7)
{
    /* tailcall */
    return sub_7f4c(arg1, arg2, arg3, arg4, arg5, arg6);
}

uint32_t j_sub_8374()
{
    /* tailcall */
    return sub_8374();
}

uint32_t j_sub_8376()
{
    /* tailcall */
    return sub_8376();
}

int32_t j_sub_8384(int32_t arg1)
{
    /* tailcall */
    return sub_8384(arg1);
}

int32_t j_sub_83c0(int32_t arg1)
{
    /* tailcall */
    return sub_83c0(arg1);
}

int32_t j_sub_83cc(int32_t arg1 @ r10)
{
    /* tailcall */
    return sub_83cc(arg1);
}

int32_t j_sub_7dbc()
{
    /* tailcall */
    return j_sub_7dbc();
}

int32_t j_sub_83ee()
{
    /* tailcall */
    return sub_83ee();
}

int32_t j_sub_83fa()
{
    /* tailcall */
    return sub_83fa();
}

int32_t j_sub_83fc()
{
    /* tailcall */
    return sub_83fc();
}

int32_t j_sub_8402(int32_t arg1, int32_t arg2)
{
    /* tailcall */
    return sub_8402(arg1, arg2);
}

int32_t j_sub_8410(int32_t arg1)
{
    /* tailcall */
    return sub_8410(arg1);
}

int32_t j_sub_8412(char arg1, char* arg2, int32_t arg3)
{
    /* tailcall */
    return sub_8412(arg1, arg2, arg3);
}

int32_t j_sub_8422()
{
    /* tailcall */
    return sub_8422();
}

int32_t j_sub_842a(int32_t arg1 @ r5)
{
    /* tailcall */
    return sub_842a(arg1);
}

int32_t j_sub_8436(int32_t arg1)
{
    /* tailcall */
    return sub_8436(arg1);
}

int32_t j_sub_8438(char* arg1, int32_t arg2)
{
    /* tailcall */
    return sub_8438(arg1, arg2);
}

int32_t j_sub_8442()
{
    /* tailcall */
    return sub_8442();
}

int32_t j_sub_8452()
{
    /* tailcall */
    return sub_8452();
}

int32_t j_sub_8454()
{
    /* tailcall */
    return sub_8454();
}

int32_t j_sub_8464(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_8464(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_8466(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_8466(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_8472(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_8472(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_8474(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_8474(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_847a(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_847a(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_847c(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5 @ r4)
{
    /* tailcall */
    return sub_847c(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_848a(int32_t arg1, int32_t arg2, int32_t arg3 @ r4, int32_t arg4 @ r5)
{
    /* tailcall */
    return sub_848a(arg1, arg2, arg3, arg4);
}

int32_t j_sub_849a(int32_t arg1)
{
    /* tailcall */
    return sub_849a(arg1);
}

int32_t j_sub_84a2(int32_t arg1)
{
    /* tailcall */
    return sub_84a2(arg1);
}

int32_t j_sub_84b2(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    /* tailcall */
    return sub_84b2(arg1, arg2, arg3, arg4);
}

int32_t j_sub_84b8(int32_t arg1, int32_t arg2, int32_t arg3, char* arg4, void* arg5 @ r4)
{
    /* tailcall */
    return sub_84b8(arg1, arg2, arg3, arg4, arg5);
}

int32_t j_sub_84ba(int32_t arg1)
{
    /* tailcall */
    return sub_84ba(arg1);
}

int32_t j_sub_84ca(void* arg1, int32_t arg2, void* arg3, void* arg4, int32_t arg6)
{
    void* arg5;
    /* tailcall */
    return sub_84ca(arg1, arg2, arg3, arg4, arg5, arg6);
}

int32_t j_sub_84dc(void* arg1, int32_t arg2, void* arg3, void* arg4, int32_t arg6)
{
    void* arg5;
    /* tailcall */
    return sub_84dc(arg1, arg2, arg3, arg4, arg5, arg6);
}

int32_t j_sub_8502(int32_t arg1)
{
    /* tailcall */
    return sub_8502(arg1);
}

int32_t j_sub_851a()
{
    /* tailcall */
    return sub_851a();
}

int32_t j_sub_8528()
{
    /* tailcall */
    return sub_8528();
}

void j_sub_852a() __noreturn
{
    /* tailcall */
    return sub_852a();
}
