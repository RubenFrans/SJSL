# SJSL - Simple Job System Library
SJSL is a small library aimed to provide a simple jobsystem for use in game- and rendering engines. It makes it easy to offload work to other threads and tries to take advantage of all cores of a modern CPU by autobalancing the jobs across the workerthreads. 

## Showcase
As a real world use case I used my Software Raytracer as a base which I wanted to improve by dividing the rendering of a frame across multiple threads.

![MTRenderer](https://user-images.githubusercontent.com/41028126/182160550-32a92619-2b9f-4ee2-b8e3-5d6c8f996078.gif)

Each job renders an horizontal strip of the frame by tracing the rays for those pixels and writing the results to the backbuffer.
When all jobs that frame are finished the front and backbuffer are flipped to show the new frame. Then the jobs are reused to render the next frame.

## Features
- Autobalancing of jobs across threads
- Automatic cleanup of detached jobs
- Automatically synchronizes all workerthreads when jobsystem is destroyed

## WIP features
- Job Stealing: WorkerThreads have a Local and a global queue. When both queue's are exhausted the thread will try to steal work from other thread's global queue as a means of auto balancing work.
  The local queue should be used for thread specific jobs E.G. UI code. Other workerthreads are not able to steal from this queue so it is guranteed to run on the assigned thread.
- Job dependencies: Jobs can have dependencies on another. This means that a job will not be executed before it's dependencies are resolved.
- Having the main thread as an optional WorkerThread

## How to use SJSL

### Running detached jobs
A detached job means running a single use job that you pass to the jobsystem to take care of it. 
The jobsystem will clean up the job after it has completed.
The disadvantage of this is that you can't check the status of the job or synchronize it with the main thread. Good writing log files or playing audio.
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};

// Scheduling a job from a lamda
js.Schedule( [] { std::cout << "Hello from thread: " << std::this_thread::get_id() << std::endl; });

// Scheduling a job using bind
js.Schedule(std::bind(&PrintNumber, 1));
```
### Running attached jobs
An attached job is a job that is passed to the jobsystem to execute but you still have a handle to set job that you can use to check the jobs status or to synchronize. E.G. you need to make sure that the job is finished before executing other code.
For example rendering a frame across multiple cores and synchronizing before flipping the front and backbuffer.
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};

// Creating the job
SJSL::Job* job{ new SJSL::Job{ [] { PrintNumber(1); } } };

// Do other stuff
//.. 

// Synchronizing the job with the current thread
job2->Join();

// cleaning up the job
delete job;
````
### A real world example - Rendering a frame of my software raytracer

#### Initializing job system
```C++
// Initializing the jobsystem
SJSL::JobSystem js{};
````
#### Initializing reusable jobs
```C++
void Elite::Renderer::InitializeRenderJobs() {
	
	m_RenderJobs = std::vector<SJSL::Job*>{};
	
	for (uint32_t r = 0; r < m_Height; r += m_RenderRowstep)
	{
		for (int i = 0; i < m_AmountOfCores; i++)
		{

			m_RenderJobs.push_back(new SJSL::Job{ std::bind(&Camera::CalculatePixelBatch, m_Camera, 0
				, r + m_RenderBatchRowAmount * i, m_RenderBatchRowAmount
				, m_Width, m_Height, m_pBackBufferPixels, m_pBackBuffer) });

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

	for (SJSL::Job* job : m_RenderJobs) {
		job->Reset();
		m_JobSystem.Schedule(job);
	}

	for (SJSL::Job* job : m_RenderJobs) {

		job->Join();
	}
}
````
### Job cleanup
```C++
Elite::Renderer::~Renderer()
{
  //...
  
	// Cleanup render jobs
	for (SJSL::Job* job : m_RenderJobs) {

		delete job;
	}

	m_RenderJobs.clear();
}
````
