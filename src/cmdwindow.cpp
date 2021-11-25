#include "cmdwindow.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <mutex>

CmdWindow::CmdWindow(int endY, int endX, int begY, int begX)
    : Window(endY, endX, begY, begX)
{
}

void CmdWindow::InitArgList(std::string args)
{
    arglist.argBuffer = args;
    arglist.curArgIdx = 0;
    arglist.curArgc = 0;
}

int CmdWindow::GetNextArg(char *arg)
{
    int argLen, i;
    if (arglist.argBuffer.size() <= arglist.curArgIdx)
        return 0;

    for (i = arglist.curArgIdx; i < arglist.argBuffer.size(); i++)
    {
        if (arglist.argBuffer[i] == ' ')
        {
            break;
        }
    }
    argLen = i - arglist.curArgIdx;
    memcpy(arg, arglist.argBuffer.c_str() + arglist.curArgIdx, argLen);
    arg[argLen] = '\0';
    arglist.curArgIdx += argLen + 1;
    return argLen;
}

int CmdWindow::PrintArgs(string input)
{
    InitArgList(input.c_str());
    char buf[1024];
    int argCount = 0;

    while ( GetNextArg(buf) != 0 ) {

        if (getcury(mWindow) == getmaxy(mWindow) - 1)
            LineClear();
        else
            wmove(mWindow, getcury(mWindow) + 1, 0);

        mvwprintw(mWindow, getcury(mWindow), 0, "arg %d : %s", argCount, buf);
        argCount++;
        
    }
    
    if (getcury(mWindow) == getmaxy(mWindow) - 1)
        LineClear();
    else
        wmove(mWindow, getcury(mWindow) + 1, 0);
    return argCount;
}

void CmdWindow::LineClear(void)
{
    wmove(mWindow, 0, 0);
    wdeleteln(mWindow);
    wmove(mWindow, getmaxy(mWindow) - 1, 0);
}

void CmdWindow::StartShell(std::mutex &mutPrintScr, std::mutex &mutGetch)
{
    char c;
    std::string s;
    int idx = 0;
    bool bPrevSpace = false;
    
    nodelay(mWindow, true);
    
    mvwprintw(mWindow, 0, 0, "> ");
    while ( true )
    {
        mutGetch.lock();
        if ( getcurx(mWindow) == getmaxx(mWindow) - 1 &&
             getcury(mWindow) == getmaxy(mWindow) - 1 )
        {
            c = wgetch(mWindow);
            LineClear();
        }
        else {
            c = wgetch(mWindow);
        }
        mutPrintScr.unlock();
        switch (c)
        {
        case ' ':
        case '\t':
            if( !bPrevSpace ) {
                s.push_back(' ');
                bPrevSpace = true;
            }
            break;

        case '\n':
            PrintArgs(s);
            wprintw(mWindow, "> ");
            s.clear();
            bPrevSpace = false;
            break;

        case ERR:
            break;

        default:
            s.push_back(c);
            bPrevSpace = false;
            break;
        }
    }
}