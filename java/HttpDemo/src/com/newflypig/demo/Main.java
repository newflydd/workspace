package com.newflypig.demo;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.URL;

public class Main {

	public static void main(String[] args) throws Exception{
		String urlPath = new String("http://139.129.53.27:22222/pushdata?TagType=1&TagIp=0");
	
		URL url=new URL(urlPath);
        HttpURLConnection httpConn=(HttpURLConnection)url.openConnection();
        
        //设置参数
        //httpConn.setDoOutput(true);     	//需要输出
        httpConn.setDoInput(true);      	//需要输入
        httpConn.setUseCaches(false);   	//不允许缓存
        httpConn.setRequestMethod("GET");   //设置GET方式连接
        
        //设置请求属性
        httpConn.setRequestProperty("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        httpConn.setRequestProperty("Accept-Encoding", "gzip, deflate, sdch");
        httpConn.setRequestProperty("Accept-Language", "en-US,en;q=0.8");
        httpConn.setRequestProperty("Connection", "Keep-Alive");// 维持长连接
        httpConn.setRequestProperty("Charset", "UTF-8");
        httpConn.setRequestProperty("Cache-Control", "max-age=0");
        httpConn.setRequestProperty("Upgrade-Insecure-Requests", "1");
        httpConn.setRequestProperty("User-Agent", "Mozilla/5.0");
        
        //设置浏览器认证
        String userCredentials = "testuser2:33abwe";
        String basicAuth = "Basic " + new String(new sun.misc.BASE64Encoder().encode(userCredentials.getBytes()));
        httpConn.setRequestProperty("Authorization", basicAuth);
        
        //连接,也可以不用明文connect，使用下面的httpConn.getOutputStream()会自动connect
        httpConn.connect();
        
        //建立输入流，向指向的URL传入参数
        //DataOutputStream dos=new DataOutputStream(httpConn.getOutputStream());                
        //dos.flush();
        //dos.close();
        
        //获得响应状态
        int resultCode=httpConn.getResponseCode();
        //System.out.println(resultCode);
        if(HttpURLConnection.HTTP_OK==resultCode){
            StringBuffer sb=new StringBuffer();
            String readLine=new String();
            BufferedReader responseReader=new BufferedReader(new InputStreamReader(httpConn.getInputStream(),"UTF-8"));
            while((readLine=responseReader.readLine())!=null){
                sb.append(readLine).append("\n");
            }
            responseReader.close();
            System.out.println(sb.toString());
        }  
	}
}
