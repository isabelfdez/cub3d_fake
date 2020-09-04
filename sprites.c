/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprites.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: isfernan <isfernan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/03 17:38:41 by isfernan          #+#    #+#             */
/*   Updated: 2020/09/04 16:57:22 by isfernan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	calculations_sprite(t_data *data, t_player *pyr)
{
	int		i;
	int		c;
	int		l;

	i = -1;
	l = 0;
	data->spr.arr = malloc(sizeof(double *) * data->spr.num);
	while (++i < data->spr.num)
		data->spr.arr[i] = malloc((sizeof(double)) * 4);
	i = 0;
	while (++l < data->l && i < data->spr.num)
	{
		c = 0;
		while (++c < data->c && i < data->spr.num)
			if (data->map[l][c] == '2')
			{
				data->spr.arr[i][0] = i;
				data->spr.arr[i][1] = (double)c + 0.5;
				data->spr.arr[i][2] = (double)l + 0.5;
				data->spr.arr[i][3] = pow(pyr->pos.x - data->spr.arr[i][1], 2)
					+ pow(pyr->pos.y - data->spr.arr[i][2], 2);
				i++;
			}
	}
}

void	sort_sprite(t_data *data)
{
	char	flag;
	int		i;
	double	*aux;

	flag = 'F';
	i = 0;
	while (flag == 'F')
	{
		i = 0;
		while (i < data->spr.num - 1)
		{
			if (data->spr.arr[i][3] < data->spr.arr[i + 1][3])
			{
				aux = data->spr.arr[i];
				data->spr.arr[i] = data->spr.arr[i + 1];
				data->spr.arr[i + 1] = aux;
			}
			i++;
		}
		flag = check_order(data);
	}
}

/*
** To calculate the sprite height and width I use the same formula
** as when I had to calculate the line height in draw_line().
** However, this time I use transY (which is actually the depth inside
** the screen) because it prevents fisheye
*/

void	transform_sprite(t_data *data, t_player *player, int i)
{
	double	det_inv;
	int		screen;
	int		x;

	det_inv = invdet(player);
	data->spr.sprx = data->spr.arr[i][1] - player->pos.x;
	data->spr.spry = data->spr.arr[i][2] - player->pos.y;
	data->spr.trnx = det_inv * (player->dir.y * data->spr.sprx
		- player->dir.x * data->spr.spry);
	data->spr.trny = det_inv * (player->cam_plane.x * data->spr.spry
		- player->cam_plane.y * data->spr.sprx);
	screen = (int)((data->resx / 2) * (1 + data->spr.trnx / data->spr.trny));
	data->spr.h = abs((int)(data->resy / data->spr.trny));
	if ((data->spr.start_y = data->resy / 2 - data->spr.h / 2) < 0)
		data->spr.start_y = 0;
	if ((data->spr.end_y = data->resy / 2 + data->spr.h / 2) >= data->resy)
		data->spr.end_y = data->resy - 1;
	data->spr.w = abs((int)(data->resy / data->spr.trny));
	if ((data->spr.start_x = screen - data->spr.w / 2) < 0)
		data->spr.start_x = 0;
	if ((data->spr.end_x = screen + data->spr.w / 2) >= data->resx)
		data->spr.end_y = data->resx - 1;
	x = data->spr.start_x - 1;
	while (++x < data->spr.end_x)
		draw_sprite(data, x, screen);
}

void	draw_sprite(t_data *data, int x, int screen)
{
	int		y;
	int		texx;
	int		texy;
	int		d;
	int		color;

	y = data->spr.start_y - 1;
	texx = (int)((x - (screen - data->spr.w / 2)) * TEX_W / data->spr.w);
	if (data->spr.trny > 0 && x > 0 && x < data->resx && data->spr.trny
		< data->spr.buff[x])
	{
		while (++y < data->spr.end_y)
		{
			d = y * 256 - data->resy * 128 + data->spr.h * 128;
			texy = ((d * TEX_H) / data->spr.h) / 256;
			color = get_tex_color_sprite(data, texx, texy);
			if (color != 0x000000)
				my_mlx_pixel_put(&data->image, x, y, color);
		}
	}
}

int		get_tex_color_sprite(t_data *data, int texx, int texy)
{
	if (texx >= 0 && texx < data->tex_w
		&& texy >= 0 && texy < data->tex_h)
	{
		return (*(int *)(data->spr.tex.texim.addr
			+ (4 * data->tex_w * texy)
			+ (4 * texx)));
	}
	return (0x0);
}
