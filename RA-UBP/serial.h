#ifndef SERIAL
#define SERIAL

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>

class Serial : public QObject  {
    Q_OBJECT

public:
    Serial();
    virtual ~Serial();
    bool enviar(QByteArray datos);
    bool enviar(int dato);
    bool isConnected();

private:
    QSerialPort serial;
};

#endif // SERIAL


// Lo siguinete es el codigo fuente utilizado para el Arduino UNO

// Hay un sitio con IDE para Arduino online: https://codebender.cc/how_it_works

//#include <Servo.h>

//Servo servo;  // Creo un objeto de la clase Servo

//boolean encendido = true;

//void setup()  {
//    servo.attach(3);  // Selecionamos el pin 3 como el pin de control para el servo

//    pinMode(13, OUTPUT);
//    pinMode(12, OUTPUT);
//    Serial.begin(9600);
//    digitalWrite(13, HIGH);
//    digitalWrite(12, HIGH);
//}

//void loop()  {
//    if (Serial.available() > 0)  {
//        int byte1 = Serial.read();  // Leemos del USB

//        servo.write(byte1);  // Escribimos en el servo
//        Serial.write(byte1);  // Devolvemos un acuse de recibo a la aplicación de control

//        if (encendido)  {
//          digitalWrite(12, LOW);
//          encendido = false;
//        }
//        else  {
//          digitalWrite(12, HIGH);
//          encendido = true;
//        }
//    }
//    delay(15);
//}

