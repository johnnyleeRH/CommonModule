#ifndef SHM_OPR_HPP
#define SHM_OPR_HPP

class ShmOpr
{
public:
	explicit ShmOpr(long size, int number);
	virtual ~ShmOpr();
	char* getWritePos();
	void IncreasePos();

private:
	bool InitSharedMemery(long size);
//	void Fillinshm(char*, int);
    int        m_shmid;
    char      *m_md_shm;
    int       *m_count;
    const  int  knumber;
};
#endif

