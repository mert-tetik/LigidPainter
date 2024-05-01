/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

*/

#include <iostream>

#include <glad/glad.h>

#include "LigidGL/LigidGL.hpp"

#include "GUI/GUI.hpp"


#if defined(_WIN32) || defined(_WIN64)
    
    //User has Windows
    #include <Windows.h>
    #include <psapi.h>


#elif defined(__APPLE__)


    #include <Cocoa/Cocoa.h>


#elif defined(__linux__)


    //User has Linux
    #include <X11/Xlib.h>
    #include <GL/glx.h>
    #include <utility>


#endif

typedef BOOL (WINAPI* PFNWGLSWAPINTERVALEXTPROC)(int interval);

void LigidGL::setSwapInterval(int interval){
    
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
    
    if (wglSwapIntervalEXT != nullptr) {
        wglSwapIntervalEXT(interval);
    }
}

void* LigidGL::getProcAddress(const char *name)
{
  void *p = (void *)wglGetProcAddress(name);
  if(p == 0 ||
    (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
    (p == (void*)-1) )
  {
    HMODULE module = LoadLibraryA("opengl32.dll");
    p = (void *)GetProcAddress(module, name);
  }

  return p;
}

bool LigidGL::isAdmin(){
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup)) {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
            isAdmin = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }
    return isAdmin == TRUE;
}

void LigidGL::forceClose(){
    ExitProcess(0);
}

size_t LigidGL::getRamUsage(){
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    
    return virtualMemUsedByMe;
}

size_t LigidGL::getTotalRam(){
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    return status.ullTotalPhys;
}

size_t LigidGL::getRamLeft(){
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    return status.ullAvailPhys;
}

void LigidGL::cleanGLErrors(){
    while (glGetError() != GL_NO_ERROR){}
}

bool LigidGL::testGLError(std::string debugTitle){
    GLenum error = glGetError();
    
    if (error != GL_NO_ERROR) {
        LGDLOG::start << "ERROR : " << debugTitle << " : OpenGL Error " << error << LGDLOG::end;
        return false;
    }

    return true;
}

#include "UTIL/Settings/Settings.hpp"

LigidWindow* LigidGL::getBoundContext(){
    if(getContext()->window.isContextCurrent())
        return &getContext()->window;
    
    if(getSecondContext()->window.isContextCurrent())
        return &getSecondContext()->window;

    if(getCopyContext()->window.isContextCurrent())
        return &getCopyContext()->window;

    return nullptr;
}