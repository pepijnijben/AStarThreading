#pragma once
#include "SDL_thread.h"
#include <thread>
#include <queue>

using namespace std;

class ThreadQueue
{
private:
	queue<function<void()>> m_tasks;
	bool finished;

	SDL_mutex* m_queLock;
	SDL_cond* m_queCond;
public:
	ThreadQueue() : finished(false), m_queLock(SDL_CreateMutex()), m_queCond(SDL_CreateCond()) { };
	~ThreadQueue() {};

	void AddJob(function<void()> task)
	{
		SDL_LockMutex(m_queLock);
		m_tasks.push(task);
		SDL_CondSignal(m_queCond);
		SDL_UnlockMutex(m_queLock);
	};

	bool IsFinished() const
	{
		return finished;
	}

	bool hasWork() const
	{
		return (m_tasks.size()>0);
	}

	void finish() {
		SDL_LockMutex(m_queLock);
		finished = true;
		SDL_CondSignal(m_queCond);
		SDL_UnlockMutex(m_queLock);
	}

	void DoNextJob()
	{
		SDL_LockMutex(m_queLock);
		
		while (m_tasks.size() == 0) {
			SDL_CondWait(m_queCond, m_queLock);
		}

		function<void()> next = m_tasks.front();
		m_tasks.pop();
		cout << m_tasks.size() << " tasks left in thread pool" << endl;
		SDL_UnlockMutex(m_queLock);
		next();
	}
};

class ThreadPool
{
private:
	vector<SDL_Thread *> m_threads;
	ThreadQueue m_queue;
public:
	ThreadPool()
	{
		for (int i = 1; i < thread::hardware_concurrency(); i++)
		{
			m_threads.push_back(SDL_CreateThread(RunTask, "PEPERNOTEN", &m_queue));
		}
	};

	~ThreadPool()
	{
		m_queue.finish();
		waitForCompletion();
		for (auto & thread : m_threads)
		{
			SDL_WaitThread(thread, NULL);
		}
	};

	void AddJob(function<void()> task)
	{
		m_queue.AddJob(task);
	}

	void waitForCompletion() {
		while (m_queue.hasWork()) {}
	}

	static int RunTask(void *ptr)
	{
		ThreadQueue * queue = (ThreadQueue*)ptr;
		while (!queue->IsFinished() || queue->hasWork())
		{
			queue->DoNextJob();
		}

		return 0;
	}
};