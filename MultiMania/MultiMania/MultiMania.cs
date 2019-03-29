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

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void Callback();

        private static bool OnPacketRecv(byte[] data, IPEndPoint ip)
        {
            if (data[0] == 100)
            {
                string s = Encoding.ASCII.GetString(data, 1, data.Length - 1);
                //Test(s);
            }
            return true;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Connect(string connectionCode, int PPS)
        {
            MessageBox.Show("Connection Code: " + connectionCode);
            return false;
        }

        [DllExport(CallingConvention.Cdecl)]
        public static bool MultiMania_Host(int PPS)
        {
            MessageBox.Show("Host");
            return false;
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
        public static extern void MultiMania_Mod_WritePlayerData(byte slot, byte[] data);

    }
}
