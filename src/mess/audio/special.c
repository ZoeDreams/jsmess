/***************************************************************************

  audio/special.c

  Functions to emulate sound hardware of Specialist MX
  ( based on code of DAI interface )

****************************************************************************/

#include "driver.h"
#include "sound/custom.h"
#include "machine/pit8253.h"
#include "includes/dai.h"
#include "streams.h"
#include "includes/special.h"

static CUSTOM_START( specimx_sh_start );
static STREAM_UPDATE( specimx_sh_update );

static sound_stream *mixer_channel;
static int specimx_input[3];

const custom_sound_interface specimx_sound_interface =
{
	specimx_sh_start,
	NULL,
	NULL
};

static CUSTOM_START( specimx_sh_start )
{
	specimx_input[0] = specimx_input[1] = specimx_input[2] = 0;
	mixer_channel = stream_create(device, 0, 1, device->machine->sample_rate, 0, specimx_sh_update);
	return (void *) ~0;
}

static STREAM_UPDATE( specimx_sh_update )
{
	INT16 channel_0_signal;
	INT16 channel_1_signal;
	INT16 channel_2_signal;

	stream_sample_t *sample_left = outputs[0];

	channel_0_signal = specimx_input[0] ? 3000 : -3000;
	channel_1_signal = specimx_input[1] ? 3000 : -3000;
	channel_2_signal = specimx_input[2] ? 3000 : -3000;

	while (samples--)
	{
		*sample_left = 0;

		/* music channel 0 */
		*sample_left += channel_0_signal;

		/* music channel 1 */
		*sample_left += channel_1_signal;

		/* music channel 2 */
		*sample_left += channel_2_signal;

		sample_left++;
	}
}

void specimx_set_input(int index, int state)
{
	if (mixer_channel!=NULL) {
		stream_update( mixer_channel );
	}
	specimx_input[index] = state;
}


