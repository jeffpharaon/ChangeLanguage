using System.Diagnostics;
using System.Runtime.InteropServices;
using Gma.System.MouseKeyHook;

namespace ChangeLanguage
{
    internal class SetConsole
    {
        [DllImport("kernel32.dll")]
        static extern IntPtr GetConsoleWindow();
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
        const int SW_HIDE = 0;
        internal static void Free()
        {
            IntPtr consoleWindow = GetConsoleWindow();
            ShowWindow(consoleWindow, SW_HIDE);
        }
    }

    internal class Background
    {
        public static void BackgroundWorker()
        {
            Task.Run(() =>
            {
                while (true)
                    Thread.Sleep(1000);
            });
        }
    }

    class Engine : Form
    {
        private IKeyboardMouseEvents _globalHook;

        public Engine()
        {
            this.ShowInTaskbar = false; 
            this.WindowState = FormWindowState.Minimized; 
            this.FormBorderStyle = FormBorderStyle.None; 
            this.Load += (s, e) => this.Hide(); 

            SetHook();
            this.FormClosing += Engine_FormClosing;
        }

        private void SetHook()
        {
            _globalHook = Hook.GlobalEvents();
            _globalHook.KeyDown += GlobalHookKeyDown;
        }

        private void GlobalHookKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Shift && e.Alt && e.KeyCode == Keys.A) 
                StartSource();
        }

        private void Engine_FormClosing(object sender, FormClosingEventArgs e)
        {
            _globalHook.KeyDown -= GlobalHookKeyDown;
            _globalHook.Dispose();
        }

        private static void StartSource(string path = "Source.exe")
        {
            try
            {
                Process process = new Process();
                process.StartInfo.FileName = path;
                process.Start();
                process.WaitForExit();
            } catch { return; }
        }

        [STAThread]
        private static void Main()
        {
            SetConsole.Free();
            Background.BackgroundWorker();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Engine());
        }
    }
}
