using System;
using System.Collections.Generic;
using System.Text;
using Thismaker.Core;
using Thismaker.Goro;
using System.Windows.Forms;
using Timer = System.Timers.Timer;
using System.Threading;
using System.Windows;
using PowerLineStatus = System.Windows.Forms.PowerLineStatus;

namespace Batman.Wpf.ViewModel
{
    public class MainViewModel:BindableBase
    {
        const int interval = 20;
        private string _percentage, _batHealth, _batLife, _chargeState;

        private WindowState _winState;
        private PowerLineStatus _lineStatus;
        private BatteryChargeStatus _batteryChargeStatus;
        private IconType _batIcon;
        private Timer updateTimer;
        private double _opacity;
        private PowerStatus _pSatus;

        private MainWindow _main;
        private CancellationToken _token;

        public MainViewModel()
        {
            
            updateTimer = new Timer(interval)
            {
                AutoReset = true,
                Enabled = true,
            };

            _pSatus = SystemInformation.PowerStatus;

            updateTimer.Elapsed += UpdateTimer_Elapsed;

            PropertyChanged += MainViewModel_PropertyChanged;
        }


        public double WindowOpacity
        {
            get { return _opacity; }
            set { SetProperty(ref _opacity, value); }
        }

        public string Percentage
        {
            get { return _percentage; }
            set { SetProperty(ref _percentage, value); }
        }

        public string BatHealth 
        {
            get { return _batHealth; }
            set { SetProperty(ref _batHealth, value); }
        }

        public string BatLife
        {
            get { return _batLife; }
            set { SetProperty(ref _batLife, value); }
        }

        public string ChargeState
        {
            get { return _chargeState; }
            set { SetProperty(ref _chargeState, value); }
        }

        public WindowState WindowState
        {
            get { return _winState; }
            set { SetProperty(ref _winState, value); }
        }

        public BatteryChargeStatus BatteryChargeStatus
        {
            get { return _batteryChargeStatus; }
            set { SetProperty(ref _batteryChargeStatus, value); }
        }

        public PowerLineStatus PowerLineStatus
        {
            get { return _lineStatus; }
            set { SetProperty(ref _lineStatus, value); }
        }

        public IconType BatIcon
        {
            get { return _batIcon; }
            set { SetProperty(ref _batIcon, value); }
        }

        private void UpdateTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            int percent = (int)(_pSatus.BatteryLifePercent * 100);
            Percentage = $"{percent}%";

            var decim = percent / 10;

            PowerLineStatus = _pSatus.PowerLineStatus;

            int startIndex = PowerLineStatus == PowerLineStatus.Online ?
                  88 : 77;

            startIndex +=decim;

            BatIcon = (IconType)startIndex;

            BatteryChargeStatus = _pSatus.BatteryChargeStatus;

            ChargeState = BatteryChargeStatus switch
            {
                BatteryChargeStatus.Charging | BatteryChargeStatus.High => "CHARGING",
                BatteryChargeStatus.Critical => "CRITICAL",
                BatteryChargeStatus.High => "HIGH",
                BatteryChargeStatus.Low => "LOW",
                BatteryChargeStatus.NoSystemBattery => "NO BATTERY",
                BatteryChargeStatus.Unknown => "UNKNOWN",
                _ => "UNKNOWN"
            };

            if (BatteryChargeStatus == BatteryChargeStatus.High && percent == 100) ChargeState = "FULLY CHARGED";

            BatLife = new TimeSpan(0, 0, _pSatus.BatteryFullLifetime).ToString();

            if (showing)
            {
                CalculateState();
            }

        }

        private void MainViewModel_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (((e.PropertyName == nameof(BatteryChargeStatus))||e.PropertyName==nameof(PowerLineStatus)) && _main!=null)
            {
                //_main.CallActivate();

                showing = true;
                elapsedMils = 0;
                WindowState = WindowState.Normal;
                WindowOpacity = 1;
            }
        }

        bool showing = false;
        int elapsedMils;
        private void CalculateState()
        {
            elapsedMils += interval;

            if (elapsedMils > 5000)
            {
                double opacity = (double)(2000 - (elapsedMils - 5000))/2000;
                WindowOpacity = opacity;

                if (WindowOpacity == 0)
                {
                    WindowState = WindowState.Minimized;
                    showing = false;
                }
            }
        }

        public void AttachWindow(MainWindow main, CancellationToken cancellationToken)
        {
            _main = main;
        }

        public void MouseEntered()
        {
            
        }

        public void MouseLeft()
        {

        }

    }
}
