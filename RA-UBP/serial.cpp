#include "serial.h"

Serial::Serial()  {

    QList<QSerialPortInfo> portsDisponibles = QSerialPortInfo::availablePorts();

    int indexArduino = 0;  // Aqui guardaremos el index del puerto serial que lleve como nombre Arduino

    for (int i=0 ; i<portsDisponibles.size() ; i++)  {

        QSerialPortInfo portInfo = portsDisponibles.at(i);

        if (portInfo.manufacturer().contains("Arduino"))
            indexArduino = i;

//        qDebug() << portInfo.description();
//        qDebug() << portInfo.manufacturer();
//        qDebug() << portInfo.portName();
//        qDebug() << portInfo.serialNumber();
//        qDebug() << portInfo.systemLocation();
//        qDebug() << portInfo.vendorIdentifier();
    }

    bool exito = true;

    if (portsDisponibles.size() > 0)  {
        serial.setPort(portsDisponibles.at(indexArduino));

        if (serial.open(QIODevice::ReadWrite))  {
            if (!serial.setBaudRate(QSerialPort::Baud9600))
                exito = false;
            if (!serial.setDataBits(QSerialPort::Data8))
                exito = false;
            if (!serial.setParity(QSerialPort::NoParity))
                exito = false;
            if (!serial.setStopBits(QSerialPort::OneStop))
                exito = false;
            if (!serial.setFlowControl(QSerialPort::NoFlowControl))
                exito = false;
        }
    }
    else
        exito = false;

    if (!exito)
        qDebug() << "No se pudo abrir el puerto COM como lectura y escritura.";
    else
        qDebug() << "El puerto serie Arduino se abrio con exito.";

}

Serial::~Serial()  {
    if (this->isConnected())  {
        serial.close();
        qDebug() << "Se cerro el serial";
    }
}

bool Serial::enviar(QByteArray datos)  {
    if ( ! this->isConnected())
        return false;

//    // Aqui lo ponemos en 90 grados
//    int valorServoHorizontalInicial = 90;
//    char c1 = (char)valorServoHorizontalInicial;
//    const char *pc1 = &c1;
//    qDebug() << serial.write(pc1, 1);

    return false;
}

bool Serial::enviar(int dato)  {
    if ( ! this->isConnected())
        return false;

    char c1 = (char)dato;
    const char *pc1 = &c1;
    int bytesEscritos = serial.write(pc1, 1);

    if (bytesEscritos == 1)
        return true;
    return false;
}

bool Serial::isConnected()  {
    return serial.isOpen();
}

