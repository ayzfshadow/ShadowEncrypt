package com.ayzf.shadowencrypt;

import android.os.Build;
import androidx.annotation.RequiresApi;

import java.util.*;

/**
 * @Author 暗影之风
 *
 * @Since 2022.01.16 12:48 周日
 *
 * @Class 影殇加密2.0版本，相对于1.0版本修改了密钥类型，可以传输非数字密钥并且修复了1.0版本密钥过长不能使用
 */
@RequiresApi(api = Build.VERSION_CODES.O)
public class ShadowEncrypt
{
    private char displacement_Operand = 1;//位移数
    private long time = System.currentTimeMillis();//当前时间
    private String key = "ayzf";//用户自定义密钥
    private byte[] keyByte = new byte[0];//密钥字节
    private boolean twoEncrypt = false;//加随机因子使相同明文加密结果成不同密文，并倒置密文全部内容
    Base64.Encoder encoder = Base64.getEncoder();
    Base64.Decoder decoder = Base64.getDecoder();

    public ShadowEncrypt(char displacement_Operand,long time,String key,boolean twoEncrypt)
    {
        this.displacement_Operand=displacement_Operand;
        this.time=time;
        this.key=key;
        keyByte = key.getBytes();
        this.twoEncrypt=twoEncrypt;
    }

    public String encrypt(String text)
    {
        if (twoEncrypt)
        {
            return new StringBuilder(new String(encoder.encode(Arrays.toString(encrypt(text.getBytes())).getBytes()))).reverse().toString();
        }
        else
        {
            return new String(encoder.encode(Arrays.toString(encrypt(text.getBytes())).getBytes()));
        }
    }
    public String decrypt(String text)
    {
        try
        {
            if (twoEncrypt)
            {
                return new String(decrypt(arraysToStringToLong(new String(decoder.decode(new StringBuilder(text).reverse().toString())))));
            }
            else
            {
                return new String(decrypt(arraysToStringToLong(new String(decoder.decode(text)))));
            }
        }
        catch (Exception e)
        {
            return "";
        }
    }

    private long[] encrypt(byte[] text)
    {
        long[] aa = new long[0];//声明密文数组
        long rand = rand();
        for (int i = 0; i < text.length; i++)//将字节数组每一个字节进行位移
        {
            int ab = text[i] << displacement_Operand;//进行位移，向左偏移指定位移数
            long ac = 0;
            if (twoEncrypt)
            {
                ac = ab + this.getKeyByte(i) + this.keyByte.length + rand;//位移内容加上密钥字节以及密钥字节长度和随机因子
            }
            else
            {
                ac = ab + this.getKeyByte(i) + this.keyByte.length;//位移内容加上密钥字节以及密钥字节长度
            }
            aa=insert(aa,ac);//将内容写入aa数组
        }
        long[] result = new long[0];
        if (twoEncrypt)
        {
            result = insert2(aa,new long[]{2993,29930,299300,rand},time);//开头插入密文识别标识和随机因子数，结尾插入时间
        }
        else
        {
            result = insert2(aa,new long[]{2993,29930,299300},time);//开头插入密文识别标识，结尾插入时间
        }
        return result;//返回最终密文字节数组
    }
    private byte[] decrypt(long[] text) throws Exception
    {
        if (text[0] == 2993 && text[1] == 29930 && text[2] == 299300)//判断该加密独有标识，不符合标识即不是该加密的密文
        {
            if ((!twoEncrypt) || (twoEncrypt && (text[text.length - 1] + 3000) > time))//判断是否开启随机因子功能，如果开启判断时间是否超时，防止抓包拷贝密文进行注入
            {
                long rand = text[3];
                long[] aa = new long[0];
                if (twoEncrypt)
                {
                    aa = delElement(delElement(delElement(delElement(text,0),0),0),0);//移除加密标识
                }
                else
                {
                    aa = delElement(delElement(delElement(text,0),0),0);//移除加密标识
                }
                long[] ab = delElement(aa,(aa.length) - 1);//移除结尾时间，获得真正密文
                byte[] result = new byte[0];
                //开始解密
                for (int i = 0; i < ab.length; i++)//每个字节
                {
                    long aab = 0;
                    if (twoEncrypt)
                    {
                        aab = ab[i] - rand - this.keyByte.length - this.getKeyByte(i);//减去随机因子和key字节以及密钥字节长度，解密第一层
                    }
                    else
                    {
                        aab = ab[i] - this.keyByte.length - this.getKeyByte(i);//减去key字节以及密钥字节长度，解密第一层
                    }
                    long aac = aab >> Long.valueOf(displacement_Operand);//进行位移，向右偏移指定位移数，解密第二层
                    result=insert(result,(byte)aac);//插入每个解密的最终明文字节
                }
                return result;
            }
            else
            {
                throw new Exception("时间过长，禁止解密");
            }
        }
        else
        {
            throw new Exception("不是本加密密文");
        }
    }
    private long getKeyByte(int index)
    {
        try
        {
            return keyByte[index];
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            return this.displacement_Operand;
        }
    }
    private static int rand()
    {
        return new Random().nextInt(1000);
    }
    private static byte[] insert(byte[] arr,byte... str)
    {
        int size = arr.length;//获取原数组长度
        int newSize = size + str.length;//原数组长度加上追加的数据的总长度
        //新建临时字符串数组
        byte[] tmp = new byte[newSize];
        //先遍历将原来的字符串数组数据添加到临时字符串数组
        for (int i = 0; i < size; i++)
        {
            tmp[i]=arr[i];
        }
        //在末尾添加上需要追加的数据
        for (int i = size; i < newSize; i++)
        {
            tmp[i]=str[i - size];
        }
        return tmp;//返回拼接完成的字符串数组
    }
    private static long[] insert(long[] arr,long... str)
    {
        int size = arr.length;//获取原数组长度
        int newSize = size + str.length;//原数组长度加上追加的数据的总长度
        //新建临时字符串数组
        long[] tmp = new long[newSize];
        //先遍历将原来的字符串数组数据添加到临时字符串数组
        for (int i = 0; i < size; i++)
        {
            tmp[i]=arr[i];
        }
        //在末尾添加上需要追加的数据
        for (int i = size; i < newSize; i++)
        {
            tmp[i]=str[i - size];
        }
        return tmp;//返回拼接完成的字符串数组
    }
    private static long[] insert2(long[] arr,long[] str,long... str2)
    {
        int size = arr.length;//获取原数组长度
        int size2 = str.length;//开头数组长度
        int newSize = size + size2 + str2.length;//原数组长度加上追加的数据的总长度
        //新建临时字符串数组
        long[] tmp = new long[newSize];
        //拼接开头
        for (int i = 0; i < size2; i++)
        {
            tmp[i]=str[i];
        }
        //先遍历将原来的字符串数组数据添加到临时字符串数组
        for (int i = size2; i < (size + size2); i++)
        {
            tmp[i]=arr[i - size2];
        }
        //在末尾添加上需要追加的数据
        for (int i = (size + size2); i < newSize; i++)
        {
            tmp[i]=str2[i - (size + size2)];
        }
        return tmp;//返回拼接完成的字符串数组
    }
    private static long[] delElement(long[] arrays,int index)
    {
        long[] result = new long[arrays.length - 1];
        boolean isOk = false;
        for (int i = 0; i < arrays.length; i++)
        {
            if (i == index)
            {
                isOk = true;
                continue;
            }
            if (isOk)
            {
                result[i-1] = arrays[i];
            }
            else
            {
                result[i] = arrays[i];
            }
        }
        return result;
    }
    private static long[] arraysToStringToLong(String str)
    {
        String[] aa = str.replace("[","").replace("]","").replaceAll(" ","").split(",");
        long[] result = new long[aa.length];
        for (int i = 0; i < result.length; i++)
        {
            result[i] = Long.valueOf(aa[i]);
        }
        return result;
    }
}