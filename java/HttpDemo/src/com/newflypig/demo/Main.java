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
        
        //���ò���
        //httpConn.setDoOutput(true);     	//��Ҫ���
        httpConn.setDoInput(true);      	//��Ҫ����
        httpConn.setUseCaches(false);   	//��������
        httpConn.setRequestMethod("GET");   //����GET��ʽ����
        
        //������������
        httpConn.setRequestProperty("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
        httpConn.setRequestProperty("Accept-Encoding", "gzip, deflate, sdch");
        httpConn.setRequestProperty("Accept-Language", "en-US,en;q=0.8");
        httpConn.setRequestProperty("Connection", "Keep-Alive");// ά�ֳ�����
        httpConn.setRequestProperty("Charset", "UTF-8");
        httpConn.setRequestProperty("Cache-Control", "max-age=0");
        httpConn.setRequestProperty("Upgrade-Insecure-Requests", "1");
        httpConn.setRequestProperty("User-Agent", "Mozilla/5.0");
        
        //�����������֤
        String userCredentials = "testuser2:33abwe";
        String basicAuth = "Basic " + new String(new sun.misc.BASE64Encoder().encode(userCredentials.getBytes()));
        httpConn.setRequestProperty("Authorization", basicAuth);
        
        //����,Ҳ���Բ�������connect��ʹ�������httpConn.getOutputStream()���Զ�connect
        httpConn.connect();
        
        //��������������ָ���URL�������
        //DataOutputStream dos=new DataOutputStream(httpConn.getOutputStream());                
        //dos.flush();
        //dos.close();
        
        //�����Ӧ״̬
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
