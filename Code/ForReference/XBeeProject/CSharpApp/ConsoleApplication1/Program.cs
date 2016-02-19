using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace ConsoleApplication1{
    class Program{
        static void Main(string[] args){
            SerialPort serialPort1 = new SerialPort();
            serialPort1.BaudRate = 9600;
            serialPort1.DataBits = 8;
            serialPort1.Handshake = Handshake.None;
            serialPort1.Parity = Parity.None;
            serialPort1.PortName = "COM6";
            serialPort1.StopBits = StopBits.One;
            serialPort1.Open();
            string toSend;
            string synchFrame = "1499";
            int json_length = 0;
            byte[] size = new byte[4];

            if (serialPort1.IsOpen)
            {
                while (true)
                {
                    toSend = "{\"message\":{\"type\":\"WD\",\"id\":\"0\"},\"network\":{\"id\":\"network_id\"},\"routing\":{\"from_high\":\"routing_from_high\",\"from_low\":\"routing_from_low\",\"to_high\":\"routing_to_high\",\"to_low\":\"routing_to_low\"},\"data\":{\"name\":\"DIMMER 2 PERCENT\",\"type\":\"data_type\",\"bytes\":\"100\"}}\0";
                    synchFrame = "1499";
                    json_length = toSend.Length;
                    size[0] = (byte)(json_length >> 24);
                    size[1] = (byte)(json_length >> 16);
                    size[2] = (byte)(json_length >> 8);
                    size[3] = (byte)(json_length);
                    serialPort1.Write(synchFrame);
                    serialPort1.Write(size, 0, 4);
                    serialPort1.Write(toSend);
                    Console.WriteLine("Data Sent! Press any button to exit.");

                    int time = Environment.TickCount;

                    while (true)
                    {
                        if (Environment.TickCount - time >= 2000) break;
                    }

                    toSend = "{\"message\":{\"type\":\"WD\",\"id\":\"0\"},\"network\":{\"id\":\"network_id\"},\"routing\":{\"from_high\":\"routing_from_high\",\"from_low\":\"routing_from_low\",\"to_high\":\"routing_to_high\",\"to_low\":\"routing_to_low\"},\"data\":{\"name\":\"DIMMER 2 PERCENT\",\"type\":\"data_type\",\"bytes\":\"0\"}}\0";
                    json_length = toSend.Length;
                    size[0] = (byte)(json_length >> 24);
                    size[1] = (byte)(json_length >> 16);
                    size[2] = (byte)(json_length >> 8);
                    size[3] = (byte)(json_length);
                    serialPort1.Write(synchFrame);
                    serialPort1.Write(size, 0, 4);
                    serialPort1.Write(toSend);
                    Console.WriteLine("Data Sent! Press any button to exit.");

                    time = Environment.TickCount;

                    while (true)
                    {
                        if (Environment.TickCount - time >= 2000) break;
                    }
                }

                //while (!Console.KeyAvailable)
                //{
                //    Console.Write("" + (char)serialPort1.ReadChar());
                //}
            }
            else
            {
                Console.WriteLine("Unable to open " + serialPort1.PortName);
            }
            while (true) ;
        }
    }
}

