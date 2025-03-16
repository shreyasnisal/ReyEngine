#include "JobSystem.hpp"


void Job::UpdateStatus(JobStatus newStatus)
{
	m_status = newStatus;
}

JobWorker::JobWorker(JobWorkerID id, JobSystem* jobSystem)
	: m_id(id)
	, m_jobSystem(jobSystem)
{
	m_thread = std::thread(&JobWorker::ThreadMain, this);
}

void JobWorker::ThreadMain()
{
	while (!m_jobSystem->m_isShuttingDown)
	{
		Job* job = m_jobSystem->ClaimJob();
		if (!job)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(1));
			continue;
		}
		job->Execute();
		m_jobSystem->MarkJobComplete(job);
	}
}

JobSystem::JobSystem(JobSystemConfig config)
	: m_config(config)
{
}

void JobSystem::Startup()
{
	m_isShuttingDown = false;

	int numWorkers = m_config.m_numWorkers;

	if (numWorkers == -1)
	{
		numWorkers = std::thread::hardware_concurrency();
	}

	CreateWorkers(numWorkers);
}

void JobSystem::BeginFrame()
{
}

void JobSystem::EndFrame()
{
}

void JobSystem::Shutdown()
{
	m_isShuttingDown = true;
	DestroyWorkers();

	m_queuedJobsMutex.lock();
	for (int jobIndex = 0; jobIndex < (int)m_queuedJobs.size(); jobIndex++)
	{
		delete m_queuedJobs[jobIndex];
	}
	m_queuedJobs.clear();
	m_queuedJobsMutex.unlock();
	
	m_claimedJobsMutex.lock();
	for (int jobIndex = 0; jobIndex < (int)m_claimedJobs.size(); jobIndex++)
	{
		delete m_claimedJobs[jobIndex];
	}
	m_claimedJobs.clear();
	m_claimedJobsMutex.unlock();

	m_completedJobsMutex.lock();
	for (int jobIndex = 0; jobIndex < (int)m_completedJobs.size(); jobIndex++)
	{
		delete m_completedJobs[jobIndex];
	}
	m_completedJobs.clear();
	m_completedJobsMutex.unlock();
}

void JobSystem::CreateWorkers(int numWorkers)
{
	m_workers.resize(numWorkers);

	for (int workerId = 0; workerId < numWorkers; workerId++)
	{
		JobWorker* worker = new JobWorker(workerId, this);
		m_workers[workerId] = worker;
	}
}

void JobSystem::DestroyWorkers()
{
	for (int workerId = 0; workerId < (int)m_workers.size(); workerId++)
	{
		m_workers[workerId]->m_thread.join();
		delete m_workers[workerId];
		m_workers[workerId] = nullptr;
	}

	m_workers.clear();
}

void JobSystem::QueueJob(Job* job)
{
	m_queuedJobsMutex.lock();
	m_queuedJobs.push_back(job);
	m_queuedJobsMutex.unlock();
}

Job* JobSystem::ClaimJob()
{
	m_queuedJobsMutex.lock();
	if (m_queuedJobs.empty())
	{
		m_queuedJobsMutex.unlock();
		return nullptr;
	}

	Job* job = m_queuedJobs.front();
	m_queuedJobs.pop_front();
	m_queuedJobsMutex.unlock();

	m_claimedJobsMutex.lock();
	m_claimedJobs.push_back(job);
	m_claimedJobsMutex.unlock();

	job->UpdateStatus(JobStatus::CLAIMED);

	return job;
}

void JobSystem::MarkJobComplete(Job* job)
{
	m_claimedJobsMutex.lock();
	for (auto jobIter = m_claimedJobs.begin(); jobIter != m_claimedJobs.end(); ++jobIter)
	{
		if (*jobIter == job)
		{
			m_claimedJobs.erase(jobIter);
			break;
		}
	}
	m_claimedJobsMutex.unlock();

	m_completedJobsMutex.lock();
	m_completedJobs.push_back(job);
	m_completedJobsMutex.unlock();

	job->UpdateStatus(JobStatus::COMPLETED);
}

Job* JobSystem::GetCompletedJob()
{
	m_completedJobsMutex.lock();
	if (m_completedJobs.empty())
	{
		m_completedJobsMutex.unlock();
		return nullptr;
	}

	Job* completedJob = m_completedJobs.front();
	m_completedJobs.pop_front();
	m_completedJobsMutex.unlock();

	completedJob->UpdateStatus(JobStatus::RETREIVED);

	return completedJob;
}
