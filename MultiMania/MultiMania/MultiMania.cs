using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MultiMania
{
    public class MultiMania
    {

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Connect(string connectionCode, int PPS)
        {
            MultiManiaConnectionHandler.Connect(connectionCode);
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Close()
        {
            MultiManiaConnectionHandler.Connection.CloseConnection();
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_IsHost()
        {
            return MultiManiaConnectionHandler.Connection.Hosting;
        }


        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Host(int PPS)
        {
            MultiManiaConnectionHandler.Host(PPS);
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_SpawnObject(short objectID, short subObject, int x, int y)
        {
            var data = new byte[12];
            Array.Copy(BitConverter.GetBytes(objectID), 0, data, 0, 2);
            Array.Copy(BitConverter.GetBytes(subObject), 0, data, 2, 2);
            Array.Copy(BitConverter.GetBytes(x), 0, data, 4, 4);
            Array.Copy(BitConverter.GetBytes(y), 0, data, 8, 4);
            MultiManiaConnectionHandler.Connection.SendData(10, data);
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static void MultiMania_Update()
        {
            if (MultiManiaConnectionHandler.Connection.Connected && !MultiManiaConnectionHandler.Bonk)
            {
                var data = new byte[25];
                MultiMania_Mod_ReadPlayerData(0, data);
                var bytes = new byte[25 + 8];
                Array.Copy(data, 0, bytes, 8, data.Length);
                Array.Copy(BitConverter.GetBytes(MultiManiaConnectionHandler.PacketCountSEND), 0, bytes, 0, 8);
                MultiManiaConnectionHandler.Connection.SendData(10, MultiManiaConnectionHandler.Compress(bytes));
                ++MultiManiaConnectionHandler.PacketCountSEND;
            }
        }


        [DllExport(CallingConvention.Cdecl)]
        public static void InitMultiMania()
        {
            //var network = new Network();
            //network.OpenConnection(GetServerAddress(), 16004);
            //network.RegisterPacketEvent(OnPacketRecv);
            //network.SendCommand(100);
            //network.CloseConnection();
            //OpenMenu();
        }

        public enum Character
        {
            Sonic = 0b0000_0001,
            Tails = 0b0000_0010,
            Knuckles = 0b0000_0100,
            Mighty = 0b0000_1000,
            Ray = 0b0001_0000
        }


        [DllImport("MultiMania-Mod.dll")]
        public static extern int OpenMenu();
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_ChangeScene(byte scene);
        [DllImport("MultiMania-Mod.dll")]
        public static extern byte MultiMania_Mod_GetScene();
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_SetCharacter(byte slot, Character character);
        [DllImport("MultiMania-Mod.dll")]
        public static extern Character MultiMania_Mod_GetCharacter(byte slot);
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_SendEvent(int errorcode);
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_SendHostConnectionCode(string connectionCode);
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_WritePlayerData(byte slot, byte[] data);
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_ReadPlayerData(byte slot, [In, Out] byte[] data);
        [DllImport("MultiMania-Mod.dll")]
        public static extern void MultiMania_Mod_SpawnObject(short objectID, short subObject, int x, int y);

    }
}
