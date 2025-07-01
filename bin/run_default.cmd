@echo off

echo "Running the default test..."
ICCAD2025_ProblemC.exe --design tests\aes_cipher_top --asap7 tests\ASAP7
echo "done!"

pause