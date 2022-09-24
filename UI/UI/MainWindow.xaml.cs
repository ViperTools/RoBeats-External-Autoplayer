using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Runtime.InteropServices;
using System.IO;
using System.IO.Pipes;
using System.Windows.Input;
using System.ComponentModel;
using System.Text.Json;
using System.Linq;

namespace UI
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct Config
    {
        [MarshalAs(UnmanagedType.Bool)]
        public bool ManualKeys = true;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public string[] Keys = new string[4] { "A", "S", "L", "Semicolon" };
    }

    enum InjectError
    {
        NONE,
        NO_ROBLOX,
        NO_WORKSPACE,
        NO_DATAMODEL
    }

    public partial class MainWindow : Window
    {
        private bool attached = false, attaching = false;
        private TextBlock attachText;

        [DllImport("RoBeats.dll")]
        public static extern int Inject();

        [DllImport("RoBeats.dll")]
        public static extern void Detach();

        [DllImport("RoBeats.dll")]
        public static extern void SetOptions(Config config);

        private static Config config = new Config();

        private static Dictionary<string, string> keyMap = new Dictionary<string, string>
        {
            { ",", "Comma" },
            { "-", "Minus" },
            { ".", "Period" },
            { "0", "Zero" },
            { "1", "One" },
            { "2", "Two" },
            { "3", "Three" },
            { "4", "Four" },
            { "5", "Five" },
            { "6", "Six" },
            { "7", "Seven" },
            { "8", "Eight" },
            { "9", "Nine" },
            { ";", "Semicolon" },
            { "=", "Equals" },
            { "[", "LeftBracket" },
            { "\\", "BackSlash" },
            { "/", "Slash" },
            { "`" ,"Backquote" }
        };

        private void Error(string err)
        {
            attaching = false;
            attached = false;
            Application.Current.Dispatcher.Invoke(() => attachText.Text = err);
            Thread.Sleep(2000);
            Application.Current.Dispatcher.Invoke(() => attachText.Text = "Attach");
        }

        private static string GetKey(string key)
        {
            return keyMap.ContainsKey(key) ? keyMap[key] : (key.Length == 1 ? key.ToUpper() : key);
        }

        private static string KeyToString(string key)
        {
            return keyMap.ContainsValue(key) ? keyMap.FirstOrDefault(e => e.Value == key).Key : key;
        }

        private static void RegisterKey(TextBox tbox, int key)
        {
            tbox.TextChanged += delegate
            {
                if (tbox.Text != "None")
                {
                    config.Keys[key] = GetKey(tbox.Text);
                    SetOptions(config);
                }
            };

            tbox.GotFocus += delegate
            {
                if (tbox.Text == "None")
                {
                    tbox.Text = "";
                }
            };

            tbox.LostFocus += delegate
            {
                if (string.IsNullOrEmpty(tbox.Text))
                {
                    tbox.Text = "None";
                }
            };
        }

        public MainWindow()
        {
            InitializeComponent();
            Topbar.PreviewMouseDown += delegate { DragMove(); };
            TextBox[] keyBoxes = { Key1, Key2, Key3, Key4 };
            RegisterKey(Key1, 0);
            RegisterKey(Key2, 1);
            RegisterKey(Key3, 2);
            RegisterKey(Key4, 3);
            ManualKeysToggle.Checked += delegate { config.ManualKeys = true; SetOptions(config); };
            ManualKeysToggle.Unchecked += delegate { config.ManualKeys = false; SetOptions(config); };
            attachText = (TextBlock)((Viewbox)AttachButton.Content).Child;
            if (File.Exists("RoBeats.config"))
            {
                JsonElement jsonElement = JsonSerializer.Deserialize<JsonElement>(File.ReadAllText("RoBeats.config"));
                JsonElement keys = jsonElement.GetProperty("Keys");
                for (int i = 0; i < keys.GetArrayLength(); i++)
                {
                    config.Keys[i] = KeyToString(keys[i].GetString());
                    keyBoxes[i].Text = config.Keys[i];
                }
                config.ManualKeys = jsonElement.GetProperty("ManualKeys").GetBoolean();
                ManualKeysToggle.IsChecked = config.ManualKeys;
                SetOptions(config);
            }
            StartServer();
        }

        void StartServer()
        {
            NamedPipeServerStream server = new("RoBeatsPipe");  
            Task.Factory.StartNew(() =>
            {
                server.WaitForConnection();
                StreamReader reader = new(server);
                while (true)
                {
                    var line = reader.ReadLine();
                    switch (line)
                    {
                        case "Initialized":
                            {
                                break;
                            }
                        case "Attached":
                            {
                                attached = true;
                                attaching = false;
                                Application.Current.Dispatcher.Invoke(() => attachText.Text = "Detach");
                                break;
                            }
                        case "Detached":
                            {
                                attached = false;
                                attaching = false;
                                Application.Current.Dispatcher.Invoke(() => attachText.Text = "Attach");
                                break;
                            }
                        default: break;
                    }
                }
            });
        }

        private void Exit(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void Minimize(object sender, RoutedEventArgs e)
        {
            WindowState = WindowState.Minimized;
        }

        public void Attach(object sender, RoutedEventArgs e)
        {
            if (attaching) return;
            if (!attached)
            {
                attachText.Text = "Attaching";
                attaching = true;
                Task.Factory.StartNew(() =>
                {
                    int code = Inject();
                    attaching = false;
                    switch (code)
                    {
                        case (int)InjectError.NONE:
                            break;
                        case (int)InjectError.NO_ROBLOX:
                            Error("NO RBLX");
                            break;
                        case (int)InjectError.NO_WORKSPACE:
                            Error("NO WS");
                            break;
                        case (int)InjectError.NO_DATAMODEL:
                            Error("NO DM");
                            break;
                        default:
                            Error($"ERROR: {code}");
                            break;
                    }
                });
            }
            else
            {
                Detach();
                attaching = false;
                attachText.Text = "Attach";
            }
        }

        void Unfocus(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Debug.WriteLine("Unfocus");
            FocusManager.SetFocusedElement(this, null);
            Keyboard.ClearFocus();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            File.WriteAllText("RoBeats.config", JsonSerializer.Serialize(config, new JsonSerializerOptions { IncludeFields = true }));
            base.OnClosing(e);
        }
    }
}
