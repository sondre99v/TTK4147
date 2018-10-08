typedef struct
{
	RT_MUTEX Mutex;
	int Priority;
	int RestorePriority;
} Resource_t;

inline void resource_init(Resource_t* resource, int priority, const char* name) {
	rt_mutex_create(&resource->Mutex, name);
	resource->Priority = priority;
}

inline void resource_lock(Resource_t* resource)
{
	rt_mutex_acquire(&resource->Mutex, TM_INFINITE);
	
	struct rt_task_info temp;
	rt_task_inquire(NULL, &temp);
	resource->RestorePriority = temp.cprio;
	
	rt_task_set_priority(NULL, resource->Priority);
}

inline void resource_release(Resource_t* resource)
{
	rt_mutex_release(&resource->Mutex);
	rt_task_set_priority(NULL, resource->RestorePriority);
}

