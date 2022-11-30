set sec1_end=42
set sec2_start=127
set sec2_end=0
set wrp_start=2
set wrp_end=12
set hdp_end=11
set wrp_bank2_start=123
set wrp_bank2_end=127
set nsbootadd=0x1800c0
echo "hardening script started"
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set bank2_secure="-ob SECWM2_PSTRT=%sec2_start% SECWM2_PEND=%sec2_end%"
set connect="-c port=SWD mode=UR --hardRst"
set connect_no_reset="-c port=SWD mode=HotPlug"
set wrp_loader="WRP2A_PSTRT=%wrp_bank2_start% WRP2A_PEND=%wrp_bank2_end%"
set wrp_sbsfu="WRP1A_PSTRT=%wrp_start% WRP1A_PEND=%wrp_end%"
set nsboot_add_set="NSBOOTADD0=%nsbootadd% NSBOOTADD1=%nsbootadd%"
set write_protect_secure="-ob %wrp_sbsfu% %wrp_loader% SECWM1_PEND=%sec1_end% HDP1_PEND=%hdp_end% HDP1EN=1 %nsboot_add_set% BOOT_LOCK=1"
%stm32programmercli% %connect% %bank2_secure%
IF %errorlevel% NEQ 0 goto :error
%stm32programmercli% %connect% %write_protect_secure%
IF %errorlevel% NEQ 0 goto :error
echo "hardening script done, press key"
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo "hardening script failed, press key"
IF [%1] NEQ [AUTO] pause
exit 1
