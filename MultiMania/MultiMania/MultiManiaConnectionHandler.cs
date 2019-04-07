using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace MultiMania
{
    public static class MultiManiaConnectionHandler
    {

        public static Network Connection = new Network();
        public static ulong PacketCountSEND = 0;
        public static ulong PacketCountRECV = 0;
        public static bool Bonk = false;

        public static void Connect(string connectionCode)
        {
            // Unregisters all events
            Connection.UnregisterAllPacketEvents();
            // Registers the receive event
            Connection.RegisterPacketEvent(OnPacketRecv);
            // Closes the connection in case its still open for whatever reason
            Connection.CloseConnection();
            // TODO: Write Patches

            Connection.OpenConnection(GetServerAddress(), 16004);
            Connection.Hosting = false;
            Connection.SendData(101, Encoding.ASCII.GetBytes(connectionCode));
        }

        public static void Host(int PPS)
        {
            // Unregisters all events
            Connection.UnregisterAllPacketEvents();
            // Registers the receive event
            Connection.RegisterPacketEvent(OnPacketRecv);
            // Closes the connection in case its still open for whatever reason
            Connection.CloseConnection();
            // TODO: Write Patches

            Connection.OpenConnection(GetServerAddress(), 16004);
            Connection.Hosting = true;
            Connection.SendCommand(100);
        }

        public static byte[] Compress(byte[] data)
        {
            var output = new MemoryStream();
            using (var stream = new DeflateStream(output, CompressionLevel.Fastest))
                stream.Write(data, 0, data.Length);
            return output.ToArray();
        }

        private static byte[] Decompress(byte[] data)
        {
            var input = new MemoryStream(data);
            var output = new MemoryStream();
            using (var stream = new DeflateStream(input, CompressionMode.Decompress))
                stream.CopyTo(output);
            input.Close();
            return output.ToArray();
        }


        private static bool OnPacketRecv(byte[] data, IPEndPoint ip)
        {
            if (data[0] == 10)
            { // Move Player
                byte[] compressed = new byte[data.Length - 1];
                Array.Copy(data, 1, compressed, 0, data.Length - 1);
                //var decompressed = Decompress(compressed);
                byte[] decompressed = Decompress(compressed);
                byte[] playerData = new byte[decompressed.Length - 8];
                Array.Copy(decompressed, 8, playerData, 0, playerData.Length);
                ulong packetID = BitConverter.ToUInt64(decompressed, 0);
                if (packetID >= PacketCountRECV)
                {
                    PacketCountRECV = packetID;
                    MultiMania.MultiMania_Mod_WritePlayerData(1, playerData);
                }
                else
                {
                    Connection.LostPacketsTotal += (int)(PacketCountRECV - packetID);
                }
            }
            if (data[0] == 13)
            { // Sync New Level
                MultiMania.MultiMania_Mod_ChangeScene(data[1]);
            }
            if (data[0] == 14)
            { // Sync New Level and Character
                byte LevelID = MultiMania.MultiMania_Mod_GetScene();
                if (LevelID > 8)
                {
                    MultiMania.MultiMania_Mod_SetCharacter(0, (MultiMania.Character)data[2]);
                    MultiMania.MultiMania_Mod_SetCharacter(1, (MultiMania.Character)data[3]);
                    //MultiMania.MultiMania_Mod_SetCharacter(2, (MultiMania.Character)data[4]);
                    //MultiMania.MultiMania_Mod_SetCharacter(3, (MultiMania.Character)data[5]);
                    MultiMania.MultiMania_Mod_ChangeScene(data[1]);
                    //Connection.SendData(18, new byte[] { data[1], data[2], data[3], data[4], data[5] });
                    Connection.SendData(18, new byte[] { data[1], data[2], data[3] });
                }
                else
                {
                    Connection.SendData(17, Encoding.ASCII.GetBytes("Partner is not in a level! Please enter into a level before Syncing/Restarting"));
                }
            }
            if (data[0] == 15)
            {// Lag Test
                Connection.SendCommand(16);
            }
            if (data[0] == 16)
            {// Lag Test
                // ms: (int)((DateTime.Now - LagTime).TotalMilliseconds)
            }
            if (data[0] == 17)
            {// Log Message
                Connection.Status = Encoding.ASCII.GetString(data, 1, data.Length - 1);
            }
            if (data[0] == 18)
            {// Reset Return
                byte LevelID = MultiMania.MultiMania_Mod_GetScene();
                if (LevelID > 8)
                {
                    MultiMania.MultiMania_Mod_SetCharacter(0, (MultiMania.Character)data[3]);
                    MultiMania.MultiMania_Mod_SetCharacter(1, (MultiMania.Character)data[2]);
                    //MultiMania.MultiMania_Mod_SetCharacter(2, (MultiMania.Character)data[4]);
                    //MultiMania.MultiMania_Mod_SetCharacter(3, (MultiMania.Character)data[5]);
                    MultiMania.MultiMania_Mod_ChangeScene(data[1]);
                }
            }
            if (data[0] == 19)
            { // Spawn Object, Doesn't work
                byte LevelID = MultiMania.MultiMania_Mod_GetScene();
                if (LevelID > 8)
                {
                    MultiMania.MultiMania_Mod_SpawnObject(BitConverter.ToInt16(data, 1), BitConverter.ToInt16(data, 3), BitConverter.ToInt32(data, 5), BitConverter.ToInt32(data, 9));
                }
            }
            if (data[0] == 20)
            { // Result Data
                byte LevelID = MultiMania.MultiMania_Mod_GetScene();
                if (LevelID > 8)
                {
                    MultiMania.MultiMania_Mod_SetResultData(BitConverter.ToInt32(data, 1), BitConverter.ToInt32(data, 5), BitConverter.ToInt32(data, 9), BitConverter.ToInt32(data, 13), 1);
                }
            }
            if (data[0] == 100)
            {
                string s = Encoding.ASCII.GetString(data, 1, data.Length - 1);
                MultiMania.MultiMania_Mod_SendHostConnectionCode(s);
                Console.WriteLine(s);
            }
            if (data[0] == 102)
            {
                string s = Encoding.ASCII.GetString(data, 1, data.Length - 1);
                string _ip = s.Split(':')[0];
                string _port = s.Split(':')[1];
                Connection.Status = "Got Partner from Server!";
                Connection.lastIP = new IPEndPoint(IPAddress.Parse(_ip), int.Parse(_port));
                // TODO: Patch
                PacketCountSEND = 0;
                PacketCountRECV = 0;
                Bonk = false;
                MultiMania.MultiMania_Mod_SendEvent(0);
            }

            if (data[0] == 103)
            { // Invalid Connection Code
                Connection.Status = "Invalid Connection Code!";
                Connection.CloseConnection();
                MultiMania.MultiMania_Mod_SendEvent(1);
            }
            return true;
        }



        public static string GetServerAddress()
        {
            var wc = new WebClient();
            string address = Encoding.ASCII.GetString(
                wc.DownloadData("http://multimania.codenamegamma.com/address.txt")
                );
            wc.Dispose();
            return address;
        }


    }
}
