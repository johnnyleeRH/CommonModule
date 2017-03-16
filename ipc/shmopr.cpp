#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shmdef.hpp"
#include "log.hpp"
#include "shmopr.hpp"

ShmOpr::ShmOpr(long size, int number)
	: m_shmid(0), m_md_shm(NULL), m_count(NULL), knumber(number) {
    InitSharedMemery(size);
}

ShmOpr::~ShmOpr() {
	shmctl(m_shmid, SHM_UNLOCK, NULL);
    if (shmdt( (void *)m_count) == -1 ) {
        ERROR("shmdt failed: %s\n", strerror(errno));
    }
}

char* ShmOpr::getWritePos() {
    return m_md_shm;
}

void ShmOpr::IncreasePos() {
    if (*m_count == knumber) {
        ERROR("Share memory overflow.");
        return;
    }
    m_md_shm += UNIT_LEN;
    *m_count += 1;
}

//void ShmOpr::Fillinshm(char* data, int len) {
//	if (*m_count == knumber) {
//		INFO("Share memory overflow.");
//		return;
//	}
//	memcpy(m_md_shm, data, len);
//	m_md_shm += len;
//	*m_count += 1;
//}

bool ShmOpr::InitSharedMemery(long size) {
    m_shmid = shmget(SHM_KEY, size, IPC_CREAT | 0666);
    if (m_shmid == -1) {
        ERROR("Created shared memery error:%d, %s\n", errno, strerror(errno) );
        return false;
    }
    else {
        INFO("SHARED_MEMERY_SIZE: %ld.\n", size);
    }

    m_count  = (int *)shmat(m_shmid, NULL, 0);
    m_md_shm = (char *)(m_count + 1);
    *m_count = 0;
    shmctl(m_shmid, SHM_LOCK, NULL);

    return true;
}
