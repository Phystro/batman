using Batman.Wpf.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Batman.Wpf
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        CancellationTokenSource _cts;
        public MainWindow()
        {
            _cts = new CancellationTokenSource();
            InitializeComponent();
            ((MainViewModel)DataContext).AttachWindow(this, _cts.Token);
        }

        public void CallActivate()
        {
            Dispatcher.Invoke(() =>
            {
                Activate();
            });
        }
    }
}
