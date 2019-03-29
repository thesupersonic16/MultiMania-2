using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace MultiMania
{
    public class Network
    {

        public UdpClient UDPSocket;
        public string Status = "No Connection";
        public byte[] PacketData = null;
        public bool Connected = false;
        public bool Hosting = false;
        public int UpBytesTotal = 0;
        public int DownBytesTotal = 0;
        public int UpPacketsTotal = 0;
        public int DownPacketsTotal = 0;
        public int LostPacketsTotal = 0;
        protected List<Func<byte[], IPEndPoint, bool>> PacketEventRegistery = new List<Func<byte[], IPEndPoint, bool>>();

        public bool OpenConnection(string ipText, string portText)
        {
            try
            {
                IPAddress address = null;
                short port = 0;
                if (!IPAddress.TryParse(ipText, out address))
                {
                    Status = "Parse Failed: Malformed IP Address";
                    return false;
                }
                if (!short.TryParse(portText, out port))
                {
                    Status = "Parse Failed: Malformed Port";
                    return false;
                }
                return OpenConnection(address, port);
            }
            catch
            {
                Status = "Parse Failed: Unknown";
                return false;
            }
        }
        public bool OpenConnection(IPAddress ip, short port)
            => OpenConnection(new IPEndPoint(ip, port));

        public bool OpenConnection(string ip, short port)
            => OpenConnection(new IPEndPoint(IPAddress.Parse(ip), port));

        public virtual bool OpenConnection(IPEndPoint ip)
        {
            try
            {
                // Close exiting connections
                if (UDPSocket != null)
                {
                    CloseConnection();
                }
                Status = "Connecting...";
                UDPSocket = new UdpClient(new IPEndPoint(IPAddress.Any, 1460));
                //UDPSocket.Connect(ip);
                lastIP = ip;
                UDPSocket.Send(new byte[] { 0x01 }, 1, ip); // Checks if the server wants to connect
                UpPacketsTotal++;
                UpBytesTotal++;
                Status = "Request sent, awaiting for response...";
                IPEndPoint sender = null;
                var data = UDPSocket.Receive(ref sender);
                lastIP = sender;
                DownBytesTotal += data.Length;
                DownPacketsTotal++;
                if (data[0] != 0x01)
                {
                    Status = "Connection Failed: Got Bad reply from server";
                    CloseConnection();
                    return false;
                }
                Status = $"Successfully Connected to Partner!";
                Connected = true;
                new Thread(new ThreadStart(Loop)).Start();
                return true;
            }
            catch (Exception ex)
            {
                try
                {
                    UDPSocket.Close();
                    UDPSocket = null;
                }
                catch { }
                Status = "Connection Failed: " + ex.Message;
                return false;
            }
        }
        public bool StartConnection(string ipText, string portText)
        {
            IPAddress address = null;
            short port = 0;
            try
            {
                if (!IPAddress.TryParse(ipText, out address))
                {
                    Status = "Parse Failed: Malformed IP Address";
                    return false;
                }
                if (!short.TryParse(portText, out port))
                {
                    Status = "Parse Failed: Malformed Port";
                    return false;
                }
            }
            catch
            {
                Status = "Parse Failed: Unknown";
                return false;
            }
            return StartConnection(address, port);
        }
        public bool StartConnection(IPAddress ip, short port)
            => StartConnection(new IPEndPoint(ip, port));

        public bool StartConnection(string ip, short port)
            => StartConnection(new IPEndPoint(IPAddress.Parse(ip), port));

        public virtual bool StartConnection(IPEndPoint ip)
        {
            try
            {
                // Close exiting connections
                if (UDPSocket != null)
                {
                    CloseConnection();
                }
                Hosting = true;
                Status = "Opening Server...";
                UDPSocket = new UdpClient(ip);
                Status = "Listening...";
                IPEndPoint sender = null;
                var data = UDPSocket.Receive(ref sender);
                DownBytesTotal += data.Length;
                DownPacketsTotal++;
                // Checks if the client wants to connect
                if (data[0] == 0x01)
                {
                    //UDPSocket.Connect(sender);
                    lastIP = sender;
                    UDPSocket.Send(new byte[] { 0x01 }, 1, sender);
                    UpPacketsTotal++;
                    UpBytesTotal++;
                }else
                {
                    Status = "Connection Failed: Got Bad request from client";
                    CloseConnection();
                    return false;
                }

                Status = $"Successfully Connected to Partner!";
                Connected = true;
                new Thread(new ThreadStart(Loop)).Start();
                return true;
            }
            catch (Exception ex)
            {
                CloseConnection();
                Status = "Connection Failed: " + ex.Message;
                return false;
            }
        }

        public virtual bool CloseConnection()
        {
            Hosting = false;
            try
            {
                if (Connected)
                {
                    Connected = false;
                    UDPSocket.Send(new byte[] { 0x02 }, 1, lastIP); // Notfiy to Disconnect
                }
            }
            catch { }
            // Close exiting connection
            if (UDPSocket != null)
            {
                try
                {
                    Connected = false;
                    UDPSocket.Close();
                    UDPSocket = null;
                    Status = "Disconnected!";
                    return true;
                }
                catch (Exception ex)
                {
                    Status = "Disconection Failed: " + ex.Message;
                    UDPSocket = null;
                    return false;
                }
            }
            Connected = false;
            return true;
        }

        protected virtual void Loop()
        {
            try
            {
                while (Connected)
                {
                    if (!Connected) // Disconnect
                    {
                        CloseConnection();
                        return;
                    }
                    if (UDPSocket.Available > 0)
                    {
                        IPEndPoint ip = null;
                        PacketData = UDPSocket.Receive(ref ip);
                        DownBytesTotal += PacketData.Length;
                        DownPacketsTotal++;
                        if (PacketData[0] == 0x01) // Connect
                        {
                            // Not accepting connections
                            //Status = $"Ignoring Client from {ip} as we are not accepting connections.";
                            //return;
                        }
                        if (PacketData[0] == 0x02) // Disconnect
                        {
                            CloseConnection();
                            Status = "Disconnected because the other client is disconnecting!";
                            return;
                        }
                        if (PacketData[0] == 0x03) // Swap Role
                        {
                            Hosting = !Hosting;
                            Status = "Your role has been swapped!";
                            return;
                        }

                        for (int i = 0; i < PacketEventRegistery.Count; ++i)
                        {
                            var callback = PacketEventRegistery[i];
                            callback(PacketData, ip);
                            if (!PacketEventRegistery.Contains(callback))
                                --i;
                        }
                    }
                    Thread.Sleep(1);
                }
            }catch (Exception ex)
            {
                CloseConnection();
                Status = "Data Handling Error: " + ex;
                Console.Beep();
            }
        }

        public byte[] GetLastPacket()
        {
            return PacketData;
        }

        public void RegisterPacketEvent(Func<byte[], IPEndPoint, bool> callback)
        {
            PacketEventRegistery.Add(callback);
        }

        public void UnregisterPacketEvent(Func<byte[], IPEndPoint, bool> callback)
        {
            PacketEventRegistery.Remove(callback);
        }

        public void UnregisterAllPacketEvents()
        {
            PacketEventRegistery.Clear();
        }

        public virtual void SendPacket(byte[] data)
        {
            if (UDPSocket == null)
            {
                LostPacketsTotal++;
                return;
            }
            UDPSocket.Send(data, data.Length, lastIP);
            UpPacketsTotal++;
            UpBytesTotal += data.Length;
        }

        public IPEndPoint lastIP = null;
        public virtual void SendData(byte command, byte[] data)
        {
            if (UDPSocket == null)
            {
                LostPacketsTotal++;
                return;
            }
            byte[] bytes = new byte[data.Length + 1];
            bytes[0] = command;
            Array.Copy(data, 0, bytes, 1, data.Length);
            UDPSocket.Send(bytes, bytes.Length, lastIP);
            UpPacketsTotal++;
            UpBytesTotal += bytes.Length;
        }


        public virtual void SendCommand(byte command)
        {
            if (UDPSocket == null)
            {
                LostPacketsTotal++;
                return;
            }
            byte[] bytes = new byte[1];
            bytes[0] = command;
            UDPSocket.Send(bytes, bytes.Length, lastIP);
            UpPacketsTotal++;
            UpBytesTotal += bytes.Length;
        }

        public virtual void SwapRole()
        {
            if (Connected)
            {
                Hosting = !Hosting;
                SendCommand(0x03); // Swap Role
                Status = "Your role has been swapped!";
            }else
            {
                Status = "You cannot swap roles with nobody";
            }
        }

        public static IPAddress GetLocalIPAddress()
        {
            var host = Dns.GetHostEntry(Dns.GetHostName());
            foreach (var ip in host.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                {
                    return ip;
                }
            }
            throw new Exception("No network adapters with an IPv4 address in the system!");
        }

    }
}
