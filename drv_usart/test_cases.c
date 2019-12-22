#include <unity.h>
#include <string.h>

#include <mock_hal_usart.h>
#include <drv_usart.c>

#define NO_EVT  0xFF

static const drv_usart_cfg_t default_config = DRV_USART_DEFAULT_CONFIG;
static drv_usart_t inst = DRV_USART_INSTANCE_GET(1);

static drv_usart_evt_t received_evt;
static const uint8_t * received_buf;

void drv_usart_handler(drv_usart_evt_t evt, const uint8_t * buf)
{
    received_evt = evt;
    received_buf = buf;
}

void setUp(void)
{
    hal_usart_init_Ignore();
    hal_usart_start_Ignore();
    hal_usart_baudrate_set_Ignore();
    hal_usart_parity_set_Ignore();
    hal_usart_stopbits_set_Ignore();
    hal_usart_rx_enable_Ignore();
    hal_usart_tx_enable_Ignore();
    hal_usart_rx_disable_Ignore();
    hal_usart_tx_disable_Ignore();
    hal_usart_evt_clear_Ignore();
    hal_usart_int_enable_Ignore();
}

void tearDown(void)
{
}

void test_init(void)
{
    USART_TypeDef * p_usart = s_instances[inst.idx];

    hal_usart_init_Expect(p_usart);
    hal_usart_start_Expect(p_usart);;
    hal_usart_baudrate_set_Expect(p_usart, default_config.baudrate);
    hal_usart_parity_set_Expect(p_usart, default_config.parity);
    hal_usart_stopbits_set_Expect(p_usart, default_config.stopbits);
    hal_usart_tx_enable_Expect(p_usart);
    hal_usart_evt_clear_Expect(p_usart, HAL_USART_EVT_TC);
    hal_usart_int_enable_Expect(p_usart, HAL_USART_INT_RX | HAL_USART_INT_TC);

    drv_usart_init(&inst, &default_config, drv_usart_handler);
}

void test_rx_one_byte(void)
{
    USART_TypeDef * p_usart = s_instances[inst.idx];
    uint8_t rx_buf;
    const uint8_t byte = 'a';

    drv_usart_init(&inst, &default_config, drv_usart_handler);
    drv_usart_rx(&inst, &rx_buf, 1);

    hal_usart_evt_mask_get_ExpectAndReturn(p_usart, HAL_USART_EVT_RX);
    hal_usart_byte_rx_ExpectAndReturn(p_usart, byte);

    received_evt = NO_EVT;
    usart_irq_handler(inst.idx);
    TEST_ASSERT_EQUAL(DRV_USART_EVT_RXDONE, received_evt);
    TEST_ASSERT_EQUAL(&rx_buf, received_buf);
    TEST_ASSERT_EQUAL(byte, rx_buf);
}

void test_tx_one_byte(void)
{
    USART_TypeDef * p_usart = s_instances[inst.idx];
    uint8_t tx_buf = 'a';

    drv_usart_init(&inst, &default_config, drv_usart_handler);

    hal_usart_byte_tx_Expect(p_usart, tx_buf);
    drv_usart_tx(&inst, &tx_buf, 1);

    hal_usart_evt_mask_get_ExpectAndReturn(p_usart, HAL_USART_EVT_TC);

    received_evt = NO_EVT;
    usart_irq_handler(inst.idx);
    TEST_ASSERT_EQUAL(DRV_USART_EVT_TXDONE, received_evt);
    TEST_ASSERT_EQUAL(&tx_buf, received_buf);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_init);
    RUN_TEST(test_rx_one_byte);
    RUN_TEST(test_tx_one_byte);

    return UNITY_END();
}
