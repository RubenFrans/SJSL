# SJSL - Simple Job System Library
SJSL is a small library aimed to provide a simple jobsystem for use in game- and rendering engines. It makes it easy to offload work to other threads and tries to take advantage of all cores of a modern CPU by autobalancing the jobs across the workerthreads. 

## Showcase
As a real world use case I used my Software Raytracer as a base which I wanted to improve by dividing the rendering of a frame across multiple threads.

[comment]: <![MTRenderer](https://user-images.githubusercontent.com/41028126/182160550-32a92619-2b9f-4ee2-b8e3-5d6c8f996078.gif)>
<img src="https://user-images.githubusercontent.com/41028126/182160550-32a92619-2b9f-4ee2-b8e3-5d6c8f996078.gif" width="100%"
     >

https://github.com/RubenFrans/MultithreadedRaytracer

Each job renders an horizontal strip of the frame by tracing the rays for those pixels and writing the results to the backbuffer.
When all jobs that frame are finished the front and backbuffer are flipped to show the new frame. Then the jobs are reused to render the next frame.

## Features
- Autobalancing of jobs across threads
- Automatic cleanup of jobs
- Automatically synchronizes all workerthreads when jobsystem is destroyed

## WIP features
- Job Stealing: WorkerThreads have a Local and a global queue. When both queue's are exhausted the thread will try to steal work from other thread's global queue as a means of auto balancing work.
  The local queue should be used for thread specific jobs E.G. UI code. Other workerthreads are not able to steal from this queue so it is guranteed to run on the assigned thread.
- Job dependencies: Jobs can have dependencies on another. This means that a job will not be executed before it's dependencies are resolved.
- Having the main thread as an optional WorkerThread

## How to use SJSL

### Running jobs
Another way of running a job is creating a shared pointer to a job yourself and pass it to the jobsystem to execute, you can still use set shared pointer to check the jobs status or to synchronize. E.G. you need to make sure that the job is finished before executing other code.
For example rendering a frame across multiple cores and synchronizing before flipping the front and backbuffer.
There is no need to cleanup jobs yourself, because of the use of shared pointers, the jobs will be automatically deleted once all references are gone.
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};

// Creating the job
std::shared_ptr<SJSL::Job> job = std::make_shared<SJSL::Job>( [] { PrintNumber(1); } );

// Do other stuff
//.. 

// Synchronizing the job with the current thread
job2->Join();
````

### Running lambda's as jobs
You can easily run a lambda as a job by just passing the lamda to the JobSystem::Schedule() method.
The jobsystem will return a shared pointer to the job you can use to synchronize other threads with or if you later need to check the status of the job.
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};

// Scheduling a job from a lamda
js.Schedule( [] { PrintNumber(1); });

// Scheduling a job using bind
js.Schedule(std::bind(&PrintNumber, 1));
```
### A real world example - Rendering a frame of my software raytracer

#### Initializing job system
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};
````
#### Initializing reusable jobs
```C++
void Elite::Renderer::InitializeRenderJobs() {
	
	m_RenderJobs = std::vector<std::shared_ptr<SJSL::Job>>{};
	
	for (uint32_t r = 0; r < m_Height; r += m_RenderRowstep)
	{
		for (int i = 0; i < m_AmountOfCores; i++)
		{

			m_RenderJobs.emplace_back(std::make_shared<SJSL::Job>( std::bind(&Camera::CalculatePixelBatch, m_Camera, 0
				, r + m_RenderBatchRowAmount * i, m_RenderBatchRowAmount
				, m_Width, m_Height, m_pBackBufferPixels, m_pBackBuffer) ));

		}
	}
}
````
### Render loop
```C++
void Elite::Renderer::Render(float totalTime,float deltaT)
{
	SDL_LockSurface(m_pBackBuffer);

	HandleInput(deltaT);
	UpdateScene(totalTime, deltaT);

	if (m_EndableMultithreadedRendering)
		ScheduleRenderJobs();
	else
		SingleThreadedRendering();

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}
````
### Scheduling Jobs
```C++
void Elite::Renderer::ScheduleRenderJobs() {

	for (std::shared_ptr<SJSL::Job> job : m_RenderJobs) {
		job->Reset();
		m_JobSystem.Schedule(job);
	}

	for (std::shared_ptr<SJSL::Job> job : m_RenderJobs) {

		job->Join();
	}
}
````
### Job cleanup
There is no need to cleanup jobs yourself, because of the use of shared pointers, the jobs will be automatically deleted once all references are gone.
