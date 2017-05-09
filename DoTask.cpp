#include "DoTask.h"
DoTask::DoTask(string id): processorID(id), queueMutex(), cv()
{
  startThread();
}

DoTask::~DoTask()
{
  
}

void DoTask::workingThread()
{
  while (true)
  {
    shared_ptr<Task> task;
    {
      unique_lock<mutex> lck(queueMutex);
      while (taskQueue.empty())
      {
        cv.wait(lck);
      }
      task = taskQueue.front();
      taskQueue.pop();
    }
    if (task)
    {
      currentTask = task;
      PROCESS_INFORMATION* pi = task->start();
      if (pi)
        WaitForSingleObject(pi->hProcess, INFINITE);
    }
  }
}

void DoTask::startThread()
{
  taskThread = thread(&DoTask::workingThread, this);
}


void DoTask::startTask(string tid, string pid, string procid, string bat, string addr, Callback cb, Callback cb2)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  processInfo.set(tid, pid, bat, addr);
  //taskid="taskID":processid="processID":coreid="processorID"
  //cout << " start task cmd = " << cmd << endl;
  string sys_cmd1 = addr + bat;
  string sys_cmd2 = addr + "point_postprocessing.bat";

  {
    unique_lock<mutex> lck(queueMutex);
    taskQueue.push(make_shared<Task>(sys_cmd1));
    taskQueue.push(make_shared<Task>(sys_cmd2));
    taskQueue.push(make_shared<Task>(bind(cb, cmd)));
    cv.notify_one();
  }
}



void DoTask::killTask(string tid, string pid, string procid, string bat, Callback cb, Callback cb2)
{
  string cmd = "taskid=\"" + tid + "\":processid=\"" + pid + "\":coreid=\"" + procid + "\"";
  if (processInfo.tid == tid && processInfo.pid == pid)
  {
    {
      unique_lock<mutex> lck(queueMutex);
      queue<shared_ptr<Task>>().swap(taskQueue);
    }
    if (currentTask)
    {
      currentTask->terminateExe();
    }
    //::TerminateThread(taskThread.native_handle(), 1);
    //taskThread.join();
    //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
    cb(cmd);
    //startThread();
  }
  else
  {
    //cmd=taskid="taskID":processid="processID":coreid="ProcessorID"
    cmd = "order=\"kill\":" + cmd;
    cb2(cmd);
  }
  
}


Task::Task(string s) : Task()
{
  cmd = s;
  cb = nullptr;
}

Task::Task(function<void()> f) : Task()
{
  cb = f;
  cmd = "";
}


Task::Task()
{
  ZeroMemory(&m_si, sizeof(m_si));
  m_si.cb = sizeof(m_si);
  ZeroMemory(&m_pi, sizeof(m_pi));
}

Task::~Task()
{
  terminateExe();
}

PROCESS_INFORMATION* Task::start()
{
  if (cmd != "")
  {
    if (exeCmd() == TRUE)
    {
      return &m_pi;
    }
  }
  if (cb != nullptr)
  {
    cb();
  }
  return nullptr;
}

BOOL Task::exeCmd()
{
  // Start the child process. 
  if (!::CreateProcessA(
      NULL, // No module name (use command line). 
      (LPSTR)cmd.c_str(), // Command line. 
      NULL,             // Process handle not inheritable. 
      NULL,             // Thread handle not inheritable. 
      FALSE,            // Set handle inheritance to FALSE. 
      0,                // No creation flags. 
      NULL,             // Use parent's environment block. 
      NULL,             // Use parent's starting directory. 
      (LPSTARTUPINFOA) &m_si,            // Pointer to STARTUPINFO structure.
      &m_pi)           // Pointer to PROCESS_INFORMATION structure.
      )
  {
    printf("CreateProcess failed (%d)/n", GetLastError());
    return 0;
  }
  return TRUE;
}

BOOL Task::terminateExe()
{
  if (m_pi.hProcess && m_pi.hThread)
  {
    DWORD dwEC = 0;
    BOOL b = GetExitCodeProcess(
      m_pi.hProcess,     // handle to the process
      &dwEC              // termination status
      );

    if (b)
    {
      TerminateProcess(m_pi.hProcess, dwEC);
    }
    CloseHandle(m_pi.hProcess);
    CloseHandle(m_pi.hThread);
  }

  ZeroMemory(&m_si, sizeof(m_si));
  m_si.cb = sizeof(m_si);
  ZeroMemory(&m_pi, sizeof(m_pi));

  return TRUE;
}