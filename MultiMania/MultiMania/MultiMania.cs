using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MultiMania
{
    public class MultiMania
    {

        public static int Timer = 0;

        [StructLayout(LayoutKind.Sequential)]
        public struct NetworkInfo
        {
            public bool Connected;
            public int UpBytesTotal;
            public int DownBytesTotal;
            public int UpPacketsTotal;
            public int DownPacketsTotal;
            public int LostPacketsTotal;
        }


        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Connect(string connectionCode, int PPS)
        {
            new Thread(() => MultiManiaConnectionHandler.Connect(connectionCode)).Start();
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
        public static bool MultiMania_GetNetworkInfo(IntPtr ptr)
        {
            if (ptr != IntPtr.Zero)
            {
                unsafe
                {
                    var info = (NetworkInfo*)ptr;
                    info->Connected = MultiManiaConnectionHandler.Connection.Connected;
                    info->UpBytesTotal = MultiManiaConnectionHandler.Connection.UpBytesTotal;
                    info->DownBytesTotal = MultiManiaConnectionHandler.Connection.DownBytesTotal;
                    info->UpPacketsTotal = MultiManiaConnectionHandler.Connection.UpPacketsTotal;
                    info->DownPacketsTotal = MultiManiaConnectionHandler.Connection.DownPacketsTotal;
                    info->LostPacketsTotal = MultiManiaConnectionHandler.Connection.LostPacketsTotal;
                }
            }
            return MultiManiaConnectionHandler.Connection.Connected;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Host(int PPS)
        {
            new Thread(() => MultiManiaConnectionHandler.Host(PPS)).Start();
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_SendSoundFX(short SoundFXID, int a2, byte a3)
        {
            var data = new byte[7];
            Array.Copy(BitConverter.GetBytes(SoundFXID), 0, data, 0, 2);
            Array.Copy(BitConverter.GetBytes(a2), 0, data, 2, 4);
            Array.Copy(BitConverter.GetBytes(a3), 0, data, 6, 1);
            MultiManiaConnectionHandler.Connection.SendData(19, data);
            return true;
        }
        
        [DllExport(CallingConvention.Cdecl)]
        public static void MultiMania_Update()
        {
            if (Timer == 0)
                Timer = 60;
            if (MultiManiaConnectionHandler.Connection.Connected && !MultiManiaConnectionHandler.Bonk)
            {
                var data = new byte[26];
                MultiMania_Mod_ReadPlayerData(0, data);
                var bytes = new byte[26 + 8];
                Array.Copy(data, 0, bytes, 8, data.Length);
                Array.Copy(BitConverter.GetBytes(MultiManiaConnectionHandler.PacketCountSEND), 0, bytes, 0, 8);
                MultiManiaConnectionHandler.Connection.SendData(10, MultiManiaConnectionHandler.Compress(bytes));
                ++MultiManiaConnectionHandler.PacketCountSEND;
                if (Timer % 4 == 0)
                {
                    var resultData = new byte[24];
                    MultiMania_Mod_ReadResultData(0, resultData);
                    MultiManiaConnectionHandler.Connection.SendData(20, resultData);
                }
            }
        }


        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_UpdatePlayer(Character character)
        {
            var data = new byte[2];
            data[0] = 1;
            data[1] = (byte)character;
            MultiManiaConnectionHandler.Connection.SendData(0x15, data);
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_UpdateStage(byte stage)
        {
            var data = new byte[2];
            data[0] = stage;
            MultiManiaConnectionHandler.Connection.SendData(14, data);
            return true;
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


        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int OpenMenu();
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_ChangeScene(byte scene);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern byte MultiMania_Mod_GetScene();
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_SetCharacter(byte slot, Character character);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern Character MultiMania_Mod_GetCharacter(byte slot);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_SendEvent(int errorcode);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int MultiMania_Mod_SetResultData(int score, int finalRings, int totalRings, int itemboxes, int playerID, byte character);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int MultiMania_Mod_ReadResultData(int playerID, [In, Out] byte[] data);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_SendHostConnectionCode(string connectionCode);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_WritePlayerData(byte slot, byte[] data);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void MultiMania_Mod_ReadPlayerData(byte slot, [In, Out] byte[] data);
        [DllImport("MultiMania-Mod.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr MultiMania_Mod_PlaySoundFX(short SoundFXID, int a2, byte a3);

    }
}
