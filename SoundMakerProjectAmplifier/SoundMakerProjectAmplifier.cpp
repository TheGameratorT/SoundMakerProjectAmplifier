#include <string>
#include <iostream>
#include <filesystem>

#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>

namespace fs = std::filesystem;
fs::path exe_dir;

void setApplicationTitle()
{
  TCHAR szNewTitle[MAX_PATH];
  StringCchPrintf(szNewTitle, MAX_PATH, TEXT("Sound Maker Project - Ear Raper"));
  SetConsoleTitle(szNewTitle);
}

void getExeDirectory()
{
  std::wstring wexe_dir;
  wexe_dir.resize(MAX_PATH);
  GetModuleFileName(NULL, wexe_dir.data(), MAX_PATH);

  exe_dir = wexe_dir;
}

int main()
{
  setApplicationTitle();
  getExeDirectory();

  std::string dir_path;
  std::cout << "Please input the sound maker project directory: \n";
  std::getline(std::cin, dir_path);
  std::cout << dir_path << "\n";

  for (const auto& file : fs::recursive_directory_iterator(dir_path))
  {
    //std::cout << file.path() << "\n";
    if (file.path().extension() == ".wav")
    {
      std::wstring wdest_wav_path = file.path().parent_path();
      wdest_wav_path.append(L"\\");
      wdest_wav_path.append(file.path().stem());
      wdest_wav_path.append(L"_new.wav");
      fs::path dest_wav_path = wdest_wav_path;

      std::wstring wdest_swav_path = file.path().parent_path();
      wdest_swav_path.append(L"\\");
      wdest_swav_path.append(file.path().stem());
      wdest_swav_path.append(L".adpcm.swav");
      fs::path dest_swav_path = wdest_swav_path;

      {
        STARTUPINFO info = { sizeof(info) };
        PROCESS_INFORMATION processInfo;

        std::wstring ffmpeg;
        ffmpeg.append(exe_dir.parent_path());
        ffmpeg.append(L"\\ffmpeg.exe");

        std::wstring cmd = L" -i \"";
        cmd.append(file.path());
        cmd.append(L"\" -filter:a \"volume = 50dB\" -y \"");
        cmd.append(wdest_wav_path);
        cmd.append(L"\"");

        if (CreateProcess(ffmpeg.c_str(), const_cast<wchar_t*>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        {
          WaitForSingleObject(processInfo.hProcess, INFINITE);
          CloseHandle(processInfo.hProcess);
          CloseHandle(processInfo.hThread);
        }
      }

      std::remove(file.path().string().c_str());
      std::rename(dest_wav_path.string().c_str(), file.path().string().c_str());

      {
        STARTUPINFO info = { sizeof(info) };
        PROCESS_INFORMATION processInfo;

        std::wstring waveconv;
        waveconv.append(exe_dir.parent_path());
        waveconv.append(L"\\waveconv.exe");

        std::wstring cmd = L" \"";
        cmd.append(file.path());
        cmd.append(L"\" -o \"");
        cmd.append(dest_swav_path);
        cmd.append(L"\"");

        if (CreateProcess(waveconv.c_str(), const_cast<wchar_t*>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
        {
          WaitForSingleObject(processInfo.hProcess, INFINITE);
          CloseHandle(processInfo.hProcess);
          CloseHandle(processInfo.hThread);
        }
      }
    }
  }
}