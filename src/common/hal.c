#include "hal.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/systick.h>

#if defined(STM32F407VG6T)

#include <libopencm3/stm32/rng.h>

/* STM32F407 24 MHz benchmark clock */
const struct rcc_clock_scale clock = {
    .pllm = 8,   //VCOin = HSE / PLLM = 1 MHz
    .plln = 192, //VCOout = VCOin * PLLN = 192 MHz
    .pllp = 8,   //PLLCLK = VCOout / PLLP = 24 MHz (low to have 0WS)
    .pllq = 4,   //PLL48CLK = VCOout / PLLQ = 48 MHz (required for USB, RNG)
    .pllr = 0,
    .hpre = RCC_CFGR_HPRE_DIV_NONE,
    .ppre1 = RCC_CFGR_PPRE_DIV_2,
    .ppre2 = RCC_CFGR_PPRE_DIV_NONE,
    .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
    .voltage_scale = PWR_SCALE1,
    .flash_config = FLASH_ACR_ICEN | FLASH_ACR_LATENCY_0WS,
    .ahb_frequency = 24000000,
    .apb1_frequency = 12000000,
    .apb2_frequency = 24000000,
};

#elif defined(STMF303VC6T)

/* STM32F303 72 MHz t-test clock */
const struct rcc_clock_scale clock =
    {
        .pllsrc = RCC_CFGR_PLLSRC_HSE_PREDIV,
        .pllmul = RCC_CFGR_PLLMUL_MUL9,
        .plldiv = RCC_CFGR2_PREDIV_NODIV,
        .usbdiv1 = false,
        .flash_waitstates = 2,
        .hpre = RCC_CFGR_HPRE_DIV_NONE,
        .ppre1 = RCC_CFGR_PPRE1_DIV_2,
        .ppre2 = RCC_CFGR_PPRE2_DIV_NONE,
        .ahb_frequency = 72e6,
        .apb1_frequency = 36e6,
        .apb2_frequency = 72e6,
};
#endif // defined(STM32F407VG6T)

static void clock_setup(void)
{
  rcc_clock_setup_pll(&clock);
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_USART2);
#ifdef STM32F407VG6T
  rcc_periph_clock_enable(RCC_RNG);
#endif
  flash_prefetch_enable();
}

static void gpio_setup(void)
{
  /* USART */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

  /* Osci triggers */
#ifdef STMF303VC6T
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, TRIGGER_OSCI | TRIGGER_PINPOINT);
  gpio_clear(GPIOA, TRIGGER_OSCI | TRIGGER_PINPOINT);
#endif
}

static void usart_setup(int baud)
{
  usart_set_baudrate(USART2, baud);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_enable(USART2);
}

#ifdef STM32F407VG6T
static void systick_setup(void)
{
  // assumes clock_setup was called with CLOCK_BENCHMARK
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_set_reload(2399999);
  systick_interrupt_enable();
  systick_counter_enable();
}
#endif

static void send_USART_str(const char *in)
{
  int i;
  for (i = 0; in[i] != 0; i++)
  {
    usart_send_blocking(USART2, *(unsigned char *)(in + i));
  }
  usart_send_blocking(USART2, '\n');
}

void hal_setup()
{
  clock_setup();
  gpio_setup();
  usart_setup(115200);
#ifdef STMF303VC6T
  cm_disable_interrupts();
#endif
#ifdef STM32F407VG6T
  systick_setup();
  rng_enable();
#endif
}

void hal_send_str(const char *in)
{
  send_USART_str(in);
}

static unsigned long long overflowcnt = 0;
void sys_tick_handler(void)
{
  ++overflowcnt;
}
uint64_t hal_get_time()
{
  return (overflowcnt + 1) * 2400000llu - systick_get_value();
}

void printcycles(const char *s, uint64_t c)
{
  char outs[32];
  hal_send_str(s);
  snprintf(outs, sizeof(outs), "%lu\n", (long unsigned)c);
  hal_send_str(outs);
}