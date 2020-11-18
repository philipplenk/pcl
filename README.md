# phil concurrency library

This is just a reaaaally small and largely useless collection of things related to concurrency, which I reused in some of my projects and extracted as a header only library to avoid duplicated work, most visibly in [paulchen332](https://github.com/philipplenk/paulchen332). 

As you can see, there are only two files and no tests at all. Whilst I do have some very basic tests written, those are far too ugly and unsightly to be unleashed onto the general public, which is why I chose to omit them from this repository.

At this point, the public version of this library consist of only two rather trivial class templates:

*pcl::monitor* is [exactly what it says on the tin](https://en.wikipedia.org/wiki/Monitor_(synchronization)). It simply pairs some arbitrary data with a [std::mutex](https://en.cppreference.com/w/cpp/thread/mutex) and tries to ensure it is not accessed without holding an appropriate lock.

*pcl::locked_queue* is an equally obvious thread safe queue. 

I don't think anyone except for me will ever or should ever use this, so I have not bothered with writing proper documentation. If you, dear reader, happen to disagree and are in need of some assistance or explanation, feel free to send me a mail and I shall do my very best to help you and amend this respository. 
