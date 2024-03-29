#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <stdio.h>
//#include <android/log.h>
#include <string>

/*
 * @Author 暗影之风
 *
 * @Since 2022.01.16 14:46
 *
 * @JNI 影殇保护核心之一网络保护，JNI/C++传输保护2.0
 */
extern "C"
{
    jboolean c_ver_init = JNI_FALSE;//C++版本独有判断，作用于是否设置了必要变量证明
    jchar displacement_Operand;//位移数
    jlong a_time = 0;//当前时间
    std::string key;//用户自定义密钥
    jboolean twoEncrypt = JNI_FALSE;//加随机因子使相同明文加密结果成不同密文，并倒置密文全部内容
    jobject encoder;
    jobject decoder;
    //__android_log_print(ANDROID_LOG_WARN, "ProjectName", "日志抬头", "日志内容");
    jint randA(JNIEnv* env)
    {
        //return new Random().nextInt(1000);
        jobject c_Random = env->NewObject(env->FindClass("java/util/Random"),env->GetMethodID(env->FindClass("java/util/Random"),"<init>","()V"));
        return env->CallIntMethod(c_Random,env->GetMethodID(env->GetObjectClass(c_Random),"nextInt","()I"));
    }

    jbyteArray insert(JNIEnv* env, jbyteArray arr, jbyteArray str, jboolean isFirst)
    {
        //arr.length;//获取原数组长度
        jsize size = env->GetArrayLength(arr);
        if (isFirst == JNI_TRUE)//此判断与insert2对应，不再赘述
        {
            size--;
        }
        //size + str.length;//原数组长度加上追加的数据的总长度
        jsize newSize = size + env->GetArrayLength(str);
        //new long[newSize];//新建临时字符串数组
        jbyteArray tmp = env->NewByteArray(newSize);
        jbyte tmpb[env->GetArrayLength(tmp)];
        jbyte arrb[env->GetArrayLength(arr)];
        jbyte strb[env->GetArrayLength(str)];
        env->GetByteArrayRegion(arr,0,env->GetArrayLength(arr),arrb);//给数组赋值
        env->GetByteArrayRegion(str,0,env->GetArrayLength(str),strb);
        for (int i = 0; i < size; i++)
        {
            tmpb[i] = arrb[i];
        }
        //在末尾添加上需要追加的数据
        for (int i = size; i < newSize; i++)
        {
            tmpb[i] = strb[i - size];
        }
        env->SetByteArrayRegion(tmp,0,env->GetArrayLength(tmp),tmpb);
        return tmp;//返回拼接完成的字符串数组
    }
    jlongArray insert2(JNIEnv* env, jlongArray arr, jlongArray str, jboolean isFirst)
    {
        //arr.length;//获取原数组长度
        jsize size = env->GetArrayLength(arr);
        if (isFirst == JNI_TRUE)//这个判断以及参数isFirst见调用本方法的部分解释
        {
            size--;
        }
        //size + str.length;//原数组长度加上追加的数据的总长度
        jsize newSize = size + env->GetArrayLength(str);
        //new long[newSize];//新建临时字符串数组
        jlongArray tmp = env->NewLongArray(newSize);
        jlong tmpb[env->GetArrayLength(tmp)];
        jlong arrb[env->GetArrayLength(arr)];
        jlong strb[env->GetArrayLength(str)];
        env->GetLongArrayRegion(arr,0,env->GetArrayLength(arr),arrb);//给数组赋值
        env->GetLongArrayRegion(str,0,env->GetArrayLength(str),strb);
        for (int i = 0; i < size; i++)
        {
            tmpb[i] = arrb[i];
        }
        //在末尾添加上需要追加的数据
        for (int i = size; i < newSize; i++)
        {
            tmpb[i] = strb[i - size];
        }
        env->SetLongArrayRegion(tmp,0,env->GetArrayLength(tmp),tmpb);
        return tmp;//返回拼接完成的字符串数组
    }
    jlongArray insert3(JNIEnv* env, jlongArray arr, jlongArray str, jlongArray str2)
    {
        //arr.length;//获取原数组长度
        jsize size = env->GetArrayLength(arr);
        //str.length;//开头数组长度
        jsize size2 = env->GetArrayLength(str);
        //size + size2 + str2.length;//原数组长度加上追加的数据的总长度
        jsize newSize = size + size2 + env->GetArrayLength(str2);
        //new long[newSize];//新建临时字符串数组
        jlongArray tmp = env->NewLongArray(newSize);
        //拼接开头
        jlong tmpb[env->GetArrayLength(tmp)];
        jlong strb[env->GetArrayLength(str)];
        jlong arrb[env->GetArrayLength(arr)];
        jlong str2b[env->GetArrayLength(str2)];
        env->GetLongArrayRegion(str,0,env->GetArrayLength(str),strb);//给数组赋值
        env->GetLongArrayRegion(arr,0,env->GetArrayLength(arr),arrb);
        env->GetLongArrayRegion(str2,0,env->GetArrayLength(str2),str2b);
        for (int i = 0; i < size2; i++)
        {
            tmpb[i] = strb[i];
        }
        //先遍历将原来的字符串数组数据添加到临时字符串数组
        for (int i = size2; i < (size + size2); i++)
        {
            tmpb[i] = arrb[i - size2];
        }
        //在末尾添加上需要追加的数据
        for (int i = (size + size2); i < newSize; i++)
        {
            tmpb[i] = str2b[i - (size + size2)];
        }
        env->SetLongArrayRegion(tmp,0,env->GetArrayLength(tmp),tmpb);
        return tmp;//返回拼接完成的字符串数组
    }
    jlongArray delElement(JNIEnv* env, jlongArray arrays, int index)
    {
        //long[] result = new long[arrays.length - 1];
        jlongArray result = env->NewLongArray(env->GetArrayLength(arrays) - 1);
        jlong resu[env->GetArrayLength(result)];
        jlong old[env->GetArrayLength(arrays)];
        env->GetLongArrayRegion(arrays,0,env->GetArrayLength(arrays),old);
        //boolean isOk = false;
        jboolean isOk = JNI_FALSE;
        for (int i = 0; i < env->GetArrayLength(arrays); i++)
        {
            if (i == index)
            {
                isOk = JNI_TRUE;
                continue;
            }
            if (isOk == JNI_TRUE)
            {
                resu[i-1] = old[i];
            }
            else
            {
                resu[i] = old[i];
            }
        }
        env->SetLongArrayRegion(result,0,env->GetArrayLength(result),resu);
        return result;
    }
    jlongArray arraysToStringToLong(JNIEnv* env, jstring str)
    {
        //str.replace("[","").replace("]","").replaceAll(" ","").split(",");
        jstring firstReplace = (jstring)env->CallObjectMethod(str,env->GetMethodID(env->GetObjectClass(str),"replace","(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Ljava/lang/String;"),env->NewStringUTF("["),env->NewStringUTF(""));
        jstring twoReplace = (jstring)env->CallObjectMethod(firstReplace,env->GetMethodID(env->GetObjectClass(firstReplace),"replace","(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Ljava/lang/String;"),env->NewStringUTF("]"),env->NewStringUTF(""));
        jstring endReplace = (jstring)env->CallObjectMethod(twoReplace,env->GetMethodID(env->GetObjectClass(twoReplace),"replaceAll","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"),env->NewStringUTF(" "),env->NewStringUTF(""));
        jobjectArray strSplit = (jobjectArray)env->CallObjectMethod(endReplace,env->GetMethodID(env->GetObjectClass(endReplace),"split","(Ljava/lang/String;)[Ljava/lang/String;"),env->NewStringUTF(","));
        //new Long[strSplit.length]
        jlongArray result = env->NewLongArray(env->GetArrayLength(strSplit));
        //for (int i = 0; i < result.length; i++)
        //{
        //    result[i] = Long.valueOf(aa[i]);
        //}
        jlong buf[env->GetArrayLength(result)];
        for (int i = 0; i < env->GetArrayLength(result); i++)
        {
            buf[i]= atol(env->GetStringUTFChars((jstring)env->GetObjectArrayElement(strSplit,i), 0));
        }
        env->SetLongArrayRegion(result,0,env->GetArrayLength(result),buf);
        return result;
    }
    jlong getKeyByte(JNIEnv* env,jint index)
    {
        size_t kl = key.size();
        if (index >= 0 && index < kl)
        {
            return key.data()[index];
        }
        else
        {
            return displacement_Operand;
        }
    }
    jlongArray encrypt2(JNIEnv* env, jbyteArray text)
    {
        jlongArray aa = env->NewLongArray(1);//声明密文数组
        jlong rand = randA(env);
        jbyte textb[env->GetArrayLength(text)];
        env->GetByteArrayRegion(text,0,env->GetArrayLength(text),textb);
        for (int i = 0; i < env->GetArrayLength(text); i++)//将字节数组每一个字节进行位移
        {
            jint ab = textb[i] << displacement_Operand;//进行位移，向左偏移指定位移数
            jlong ac = 0;
            if (twoEncrypt == JNI_TRUE)
            {
                ac = ab + getKeyByte(env,i) + key.size() + rand;//位移内容加上密钥字节以及密钥字节长度和随机因子
            }
            else
            {
                ac = ab + getKeyByte(env,i) + key.size();//位移内容加上密钥字节以及密钥字节长度
            }
            jlongArray re = env->NewLongArray(1);
            jlong reb[env->GetArrayLength(re)];
            reb[0] = ac;
            env->SetLongArrayRegion(re,0,env->GetArrayLength(re),reb);
            //这里判断0的原因是第一次使用for，因为C++的GetArrayLength方法不支持数组为0，判断内的方法源代码将原本新建数组长度0写成了1
            //那样会造成原本数组长度多1，第一个元素为空，影响程序最终运行效果，所以调用方法的源代码获取到新建的1长度数组后减1，就正好是0开始
            //但是只能减一次，否则全减就全部写第一个元素里面了，所以判断内方法参数最后一个jboolean就是减第一次的判断
            if (i == 0)
            {
                aa = insert2(env,aa,re,JNI_TRUE);//内容写入aa数组
            }
            else
            {
                aa = insert2(env,aa,re,JNI_FALSE);//内容写入aa数组
            }
        }
        jlongArray result = env->NewLongArray(0);
        if (twoEncrypt == JNI_TRUE)
        {
            jlongArray resu = env->NewLongArray(4);
            jlong resub[env->GetArrayLength(resu)];
            resub[0] = 2993;
            resub[1] = 29930;
            resub[2] = 299300;
            resub[3] = rand;
            env->SetLongArrayRegion(resu,0,env->GetArrayLength(resu),resub);
            jlongArray resul = env->NewLongArray(1);
            jlong resulb[env->GetArrayLength(resul)];
            resulb[0] = a_time;
            env->SetLongArrayRegion(resul,0,env->GetArrayLength(resul),resulb);
            result = insert3(env,aa,resu,resul);//开头插入密文识别标识和随机因子数，结尾插入时间
        }
        else
        {
            jlongArray resu = env->NewLongArray(3);
            jlong resub[env->GetArrayLength(resu)];
            resub[0] = 2993;
            resub[1] = 29930;
            resub[2] = 299300;
            env->SetLongArrayRegion(resu,0,env->GetArrayLength(resu),resub);
            jlongArray resul = env->NewLongArray(1);
            jlong resulb[env->GetArrayLength(resul)];
            resulb[0] = a_time;
            env->SetLongArrayRegion(resul,0,env->GetArrayLength(resul),resulb);
            result = insert3(env,aa,resu,resul);//开头插入密文识别标识，结尾插入时间
        }
        return result;//返回最终密文字节数组
    }
    jbyteArray decrypt2(JNIEnv* env, jlongArray text)
    {
        jlong textb[env->GetArrayLength(text)];
        env->GetLongArrayRegion(text,0,env->GetArrayLength(text),textb);
        if (textb[0] == 2993 && textb[1] == 29930 && textb[2] == 299300)//判断该加密独有标识，不符合标识即不是该加密的密文
        {
            if ((twoEncrypt == JNI_FALSE) || (twoEncrypt == JNI_TRUE && (textb[env->GetArrayLength(text) - 1] + 3000) > a_time))
            {
                jlong rand = textb[3];
                jlongArray aa = env->NewLongArray(0);
                if (twoEncrypt == JNI_TRUE)
                {
                    aa = delElement(env,delElement(env,delElement(env,delElement(env,text,0),0),0),0);//移除加密标识
                }
                else
                {
                    aa = delElement(env,delElement(env,delElement(env,text,0),0),0);//移除加密标识
                }
                jlongArray ab = delElement(env,aa,(env->GetArrayLength(aa)) - 1);//移除结尾时间，获得真正密文
                jbyteArray result = env->NewByteArray(1);
                jlong abb[env->GetArrayLength(ab)];
                env->GetLongArrayRegion(ab,0,env->GetArrayLength(ab),abb);
                //开始解密
                for (int i = 0; i < env->GetArrayLength(ab); i++)
                {
                    jlong aab = 0;
                    if (twoEncrypt == JNI_TRUE)
                    {
                        aab = abb[i] - rand - key.size() - getKeyByte(env,i);//减去随机因子和key字节以及密钥字节长度，解密第一层
                    }
                    else
                    {
                        aab = abb[i] - key.size() - getKeyByte(env,i);//减去key字节以及密钥字节长度,解密第一层
                    }
                    jlong aac = aab >> displacement_Operand;//进行位移，向右偏移指定位移数，解密第二层
                    jbyteArray resu = env->NewByteArray(1);
                    jbyte resub[env->GetArrayLength(resu)];
                    resub[0] = (jbyte)aac;
                    env->SetByteArrayRegion(resu,0,env->GetArrayLength(resu),resub);
                    if (i == 0)//这里的判断for也是与加密部分的对应，不再赘述
                    {
                        result = insert(env,result,resu,JNI_TRUE);//插入每个解密的最终明文字节
                    }
                    else
                    {
                        result = insert(env,result,resu,JNI_FALSE);//插入每个解密的最终明文字节
                    }
                }
                return result;
            }
            else
            {
//                jbyteArray exception = env->NewByteArray(27);
//                jbyte eb[env->GetArrayLength(exception)];
//                eb[0] = -26;
//                eb[1] = -105;
//                eb[2] = -74;
//                eb[3] = -23;
//                eb[4] = -105;
//                eb[5] = -76;
//                eb[6] = -24;
//                eb[7] = -65;
//                eb[8] = -121;
//                eb[9] = -23;
//                eb[10] = -107;
//                eb[11] = -65;
//                eb[12] = -17;
//                eb[13] = -68;
//                eb[14] = -116;
//                eb[15] = -25;
//                eb[16] = -90;
//                eb[17] = -127;
//                eb[18] = -26;
//                eb[19] = -83;
//                eb[20] = -94;
//                eb[21] = -24;
//                eb[22] = -89;
//                eb[23] = -93;
//                eb[24] = -27;
//                eb[25] = -81;
//                eb[26] = -122;
//                env->SetByteArrayRegion(exception,0,env->GetArrayLength(exception),eb);//返回"时间过长，禁止解密"字节数组
//                return exception;
                jstring exception = env->NewStringUTF("时间过长，禁止解密");
                return (jbyteArray)env->CallObjectMethod(text,env->GetMethodID(env->FindClass("java/lang/String"),"getBytes","()[B"));
            }
        }
        else
        {
//            jbyteArray exception = env->NewByteArray(21);
//            jbyte eb[env->GetArrayLength(exception)];
//            eb[0] = -28;
//            eb[1] = -72;
//            eb[2] = -115;
//            eb[3] = -26;
//            eb[4] = -104;
//            eb[5] = -81;
//            eb[6] = -26;
//            eb[7] = -100;
//            eb[8] = -84;
//            eb[9] = -27;
//            eb[10] = -118;
//            eb[11] = -96;
//            eb[12] = -27;
//            eb[13] = -81;
//            eb[14] = -122;
//            eb[15] = -27;
//            eb[16] = -81;
//            eb[17] = -122;
//            eb[18] = -26;
//            eb[19] = -106;
//            eb[20] = -121;
//            env->SetByteArrayRegion(exception,0,env->GetArrayLength(exception),eb);//返回"不是本加密密文"字节数组
//            return exception;
            jstring exception = env->NewStringUTF("不是本加密密文");
            return (jbyteArray)env->CallObjectMethod(exception,env->GetMethodID(env->GetObjectClass(exception),"getBytes","()[B"));
        }
    }

    jstring encrypt(JNIEnv* env, jstring text)
    {
        //Base64.getEncoder();
        encoder = env->CallStaticObjectMethod(env->FindClass("java/util/Base64"),env->GetStaticMethodID(env->FindClass("java/util/Base64"),"getEncoder","()Ljava/util/Base64$Encoder;"));
        if (twoEncrypt == JNI_TRUE)
        {
            //text.getBytes();
            jbyteArray tb = (jbyteArray)env->CallObjectMethod(text,env->GetMethodID(env->GetObjectClass(text),"getBytes","()[B"));
            //encrypt(tb);
            jlongArray enc = encrypt2(env,tb);
            //Arrays.toString(enc);
            jstring arrEnc = (jstring)env->CallStaticObjectMethod(env->FindClass("java/util/Arrays"),env->GetStaticMethodID(env->FindClass("java/util/Arrays"),"toString","([J)Ljava/lang/String;"),enc);
            //enc.getBytes();
            jbyteArray arrEncB = (jbyteArray)env->CallObjectMethod(arrEnc,env->GetMethodID(env->GetObjectClass(arrEnc),"getBytes","()[B"));
            //encoder.encode(arrEncB);
            jbyteArray encode = (jbyteArray)env->CallObjectMethod(encoder,env->GetMethodID(env->GetObjectClass(encoder),"encode","([B)[B"),arrEncB);
            //new String(encode);
            jstring n_str = (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),encode);
            //new StringBuilder(n_str);
            jobject c_strBui = env->NewObject(env->FindClass("java/lang/StringBuilder"),env->GetMethodID(env->FindClass("java/lang/StringBuilder"),"<init>","(Ljava/lang/String;)V"),n_str);
            //c_strBui.reverse();
            jobject reverse = env->CallObjectMethod(c_strBui,env->GetMethodID(env->GetObjectClass(c_strBui),"reverse","()Ljava/lang/StringBuilder;"));
            //reverse.toString();
            jstring to_string = (jstring)env->CallObjectMethod(reverse,env->GetMethodID(env->GetObjectClass(reverse),"toString","()Ljava/lang/String;"));
            std::string str;
            str.append("==Shadow Encrypt==");
            str.append(env->GetStringUTFChars(to_string,0));
            return env->NewStringUTF(str.c_str());
        }
        else
        {
            //text.getBytes();
            jbyteArray tb = (jbyteArray)env->CallObjectMethod(text,env->GetMethodID(env->GetObjectClass(text),"getBytes","()[B"));
            //encrypt(tb);
            jlongArray enc = encrypt2(env,tb);
            //Arrays.toString(enc);
            jstring arrEnc = (jstring)env->CallStaticObjectMethod(env->FindClass("java/util/Arrays"),env->GetStaticMethodID(env->FindClass("java/util/Arrays"),"toString","([J)Ljava/lang/String;"),enc);
            //enc.getBytes();
            jbyteArray arrEncB = (jbyteArray)env->CallObjectMethod(arrEnc,env->GetMethodID(env->GetObjectClass(arrEnc),"getBytes","()[B"));
            //encoder.encode(arrEncB);
            jbyteArray encode = (jbyteArray)env->CallObjectMethod(encoder,env->GetMethodID(env->GetObjectClass(encoder),"encode","([B)[B"),arrEncB);
            //new String(encode);
            jstring n_str = (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),encode);
            //return n_str;
            std::string str;
            str.append("==Shadow Encrypt==");
            str.append(env->GetStringUTFChars(n_str,0));
            return env->NewStringUTF(str.c_str());
        }
    }
    jstring decrypt(JNIEnv* env, jstring text)
    {
        //if (!text.contains("==Shadow Encrypt=="))
        if (env->CallBooleanMethod(text,env->GetMethodID(env->GetObjectClass(text),"contains","(Ljava/lang/CharSequence;)Z"),env->NewStringUTF("==Shadow Encrypt==")) == JNI_FALSE)
        {
            return NULL;
        }
        //Base64.getDecoder();
        decoder = env->CallStaticObjectMethod(env->FindClass("java/util/Base64"),env->GetStaticMethodID(env->FindClass("java/util/Base64"),"getDecoder","()Ljava/util/Base64$Decoder;"));
        if (twoEncrypt == JNI_TRUE)
        {
            //text.replaceFirst("==Shadow Encrypt==","");
            jstring text2 = (jstring)env->CallObjectMethod(text,env->GetMethodID(env->GetObjectClass(text),"replaceFirst","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"),env->NewStringUTF("==Shadow Encrypt=="),env->NewStringUTF(""));
            //new StringBuilder(text2);
            jobject c_strBui = env->NewObject(env->FindClass("java/lang/StringBuilder"),env->GetMethodID(env->FindClass("java/lang/StringBuilder"),"<init>","(Ljava/lang/String;)V"),text2);
            //c_strBui.reverse();
            jobject reverse = env->CallObjectMethod(c_strBui,env->GetMethodID(env->GetObjectClass(c_strBui),"reverse","()Ljava/lang/StringBuilder;"));
            //reverse.toString();
            jstring to_string = (jstring)env->CallObjectMethod(reverse,env->GetMethodID(env->GetObjectClass(reverse),"toString","()Ljava/lang/String;"));
            //decoder.decode(to_string);
            jbyteArray decode = (jbyteArray)env->CallObjectMethod(decoder,env->GetMethodID(env->GetObjectClass(decoder),"decode","(Ljava/lang/String;)[B"),to_string);
            //new String(decode);
            jstring n_str = (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),decode);
            //arraysToStringToLong(n_str);
            jlongArray attl = arraysToStringToLong(env,n_str);
            //decrypt2(attl);
            jbyteArray dec = decrypt2(env,attl);
            //return new String(dec);
            return (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),dec);
        }
        else
        {
            //text.replaceFirst("==Shadow Encrypt==","");
            jstring text2 = (jstring)env->CallObjectMethod(text,env->GetMethodID(env->GetObjectClass(text),"replaceFirst","(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"),env->NewStringUTF("==Shadow Encrypt=="),env->NewStringUTF(""));
            //decoder.decode(text2);
            jbyteArray decode = (jbyteArray)env->CallObjectMethod(decoder,env->GetMethodID(env->GetObjectClass(decoder),"decode","(Ljava/lang/String;)[B"),text2);
            //new String(decode);
            jstring n_str = (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),decode);
            //arraysToStringToLong(n_str);
            jlongArray attl = arraysToStringToLong(env,n_str);
            //decrypt2(attl);
            jbyteArray dec = decrypt2(env,attl);
            //return new String(dec);
            return (jstring)env->NewObject(env->FindClass("java/lang/String"),env->GetMethodID(env->FindClass("java/lang/String"),"<init>","([B)V"),dec);
        }
    }

    JNIEXPORT void JNICALL Java_com_ayzf_shadowencrypt_ShadowApp_init(JNIEnv* env, jclass object, jchar p_displacement_Operand, jlong p_time, jstring p_key, jboolean p_twoEncrypt)
    {
        displacement_Operand = p_displacement_Operand;
        a_time = p_time;
        key.clear();
        key.append(env->GetStringUTFChars(p_key,0));
        twoEncrypt = p_twoEncrypt;
        c_ver_init = JNI_TRUE;//设置标识，表示加载变量已设置，可以使用加解密
    }

    JNIEXPORT jstring JNICALL Java_com_ayzf_shadowencrypt_ShadowApp_encrypt(JNIEnv* env, jclass object, jstring text)
    {
        if (c_ver_init == JNI_FALSE)
        {
            return env->NewStringUTF("未完成init加载，不能使用加密");
        }
        return encrypt(env,text);
    }

    JNIEXPORT jstring JNICALL Java_com_ayzf_shadowencrypt_ShadowApp_decrypt(JNIEnv* env, jclass object, jstring text)
    {
        if (c_ver_init == JNI_FALSE)
        {
            return env->NewStringUTF("未完成init加载，不能使用解密");
        }
        return decrypt(env,text);
    }
}
