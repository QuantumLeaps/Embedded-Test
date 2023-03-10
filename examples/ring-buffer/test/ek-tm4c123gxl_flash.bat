::============================================================================
:: Batch file to program the flash of EK-TM4C123GXL
::
:: NOTE: requires the LMFlash programmer (included in QTools for Windows)
::
@echo off
setlocal

@echo Load a given binary file to the flash of EK-TM4C123GXL
@echo usage:   flash binary-file
@echo example: flash build_ek-tm4c123gxl\test.bin

if ["%~1"]==[""] (
    @echo The binary file missing
    @goto end
)
if not exist %~s1 (
    @echo The binary file '%1' does not exist
    @goto end
)

::----------------------------------------------------------------------------
:: NOTE: The following symbol LMFLASH assumes that LMFlash.exe can
:: be found on the PATH. You might need to adjust this symbol to the
:: location of the LMFlash utility on your machine
::
LMFlash.exe -q ek-tm4c123gxl %1

@echo
@echo Reset the board MANUALLY!
@pause

:end

endlocal
