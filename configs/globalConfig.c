/**
 * @file  GlobalConfig.c
 * @brief ȫ�ֱ�������
 * @details ����FreeRTOS��������ȫ�ֱ����Ķ���
 * @author konbakuyomu
 * @date 2024-09-13
 * @version 1.0
 * @copyright Copyright (c) 2024 konbakuyomu
 */

#include "globalConfig.h"

/*---------------------- ���� ----------------------*/
uint8_t hostReceiveBuffer[HOST_BUFFER_LENGTH];
uint8_t hostSendBuffer[HOST_BUFFER_LENGTH];

/**
 * @brief ��װ�жϴ�����
 * @param pstcConfig �ж����ýṹ��
 * @param u32Priority �ж����ȼ�
 * @return void
 */
void installInterruptHandler(const stc_irq_signin_config_t *pstcConfig, uint32_t u32Priority)
{
    if (NULL != pstcConfig) {
        (void)INTC_IrqSignIn(pstcConfig);
        NVIC_ClearPendingIRQ(pstcConfig->enIRQn);
        NVIC_SetPriority(pstcConfig->enIRQn, u32Priority);
        NVIC_EnableIRQ(pstcConfig->enIRQn);
    }
}

/**
 * @brief ��ʼ����̬������(����)
 * @param buffer ָ�� uint8_t ָ���ָ�룬���ڴ洢������ڴ��ַ
 * @param size Ҫ������ڴ��С
 * @return �ɹ�ʱ���ط�����ڴ��ַ��ʧ��ʱ���� NULL
 */
uint8_t *createDynamicBuffer(uint8_t **buffer, size_t size)
{
    if (buffer == NULL || size == 0) {
        return NULL;
    }

    *buffer = (uint8_t *)mymalloc(size);

    if (*buffer != NULL) {
        memset(*buffer, 0, size);
        return *buffer;
    } else {
        // �ڴ����ʧ�ܣ�������������Ӵ��������
        // ���磺��ӡ������Ϣ�򴥷�ϵͳ����
        return NULL;
    }
}