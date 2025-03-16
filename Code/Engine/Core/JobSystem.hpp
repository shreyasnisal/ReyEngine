#pragma once

#include <deque>
#include <mutex>
#include <vector>


typedef unsigned int JobWorkerID;
constexpr JobWorkerID JOBWORKERID_INVALID = 0xFFFFFFFF;

class JobSystem;

enum class JobStatus
{
	CREATED,
	QUEUED,
	CLAIMED,
	COMPLETED,
	RETREIVED
};

struct JobSystemConfig
{
public:
	int m_numWorkers = -1; // if numWorkers == -1, one thread per core will be created

};

class Job
{
public:
	virtual ~Job() = default;
	virtual void Execute() = 0;
	void UpdateStatus(JobStatus newStatus);

public:
	std::atomic<JobStatus> m_status = JobStatus::CREATED;
	std::atomic<unsigned int> m_jobBitFlags = 0x1;
};

class JobWorker
{
public:
	JobWorker(JobWorkerID id, JobSystem* jobSystem);
	void ThreadMain();

public:
	std::atomic<JobWorkerID> m_id = JOBWORKERID_INVALID;
	JobSystem* m_jobSystem = nullptr;
	std::thread m_thread;
	unsigned int m_workerBitFlags = 0x1;
};

class JobSystem
{
public:
	~JobSystem() = default;
	JobSystem() = default;
	JobSystem(JobSystemConfig config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void CreateWorkers(int numWorkers);
	void DestroyWorkers();

	void QueueJob(Job* job);
	Job* ClaimJob();
	void MarkJobComplete(Job* job);
	Job* GetCompletedJob();


public:
	JobSystemConfig m_config;
	std::vector<JobWorker*> m_workers;
	std::atomic<bool> m_isShuttingDown = false;

	std::mutex m_queuedJobsMutex;
	std::deque<Job*> m_queuedJobs;

	std::mutex m_claimedJobsMutex;
	std::deque<Job*> m_claimedJobs;

	std::mutex m_completedJobsMutex;
	std::deque<Job*> m_completedJobs;
};

