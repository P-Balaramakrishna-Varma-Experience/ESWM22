#include <Crypto.h>
#include <AES.h>
#include <SHA256.h>


char key[] = "hVmYq3s6v9y$B&E)H@McQfTjWnZr4u7w";
//uint8_t slidePressure = (uint8_t)atoi(key);

void setup()
{
  Serial.begin(115200);
}

void loop()
{
    while(Serial.available() == 0){
    }
    String Data = Serial.readString();
    String Ans = Secure_it(Data);
    Serial.println(Ans);
}

String Hash_val(String data)
{
  int len = data.length();
  char InBuf[len];
  data.toCharArray(InBuf, len);


  SHA256 sha256;
  char OutBuf[sha256.hashSize()];

  sha256.reset();
  sha256.update(InBuf, len);
  sha256.finalize(OutBuf, sha256.hashSize());

  String Ans = String(OutBuf);
  return Ans;
}

String Aes_val(String data)
{
  int len =  data.length();
  char InBuf[len], OutBuf[len];
  data.toCharArray(InBuf, len); 

  AES256 aes256;
  aes256.setKey((uint8_t*)key, aes256.keySize());
  aes256.encryptBlock((uint8_t*)OutBuf, (uint8_t*)InBuf);

  String Ans = String(OutBuf);
  return Ans;
}

String Secure_it(String Data)
{
  String hash_value = Hash_val(Data);
  String Encrypted = Aes_val(Data + ".." + hash_value);
  return Encrypted;
}