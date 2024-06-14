import React, { useEffect, useState } from "react";
import axios from 'axios';
import * as cheerio from 'cheerio';

import { Chip, Avatar, CircularProgress } from "@mui/material";

export default function VSCodeExtension({ id }) {

  const [iconSrc, SetIconSrc] = useState();

  const link = `https://marketplace.visualstudio.com/items?itemName=${id}`;

  // Icon scrapping
  const GetIconSrc = async () => {
    const { data } = await axios(link);
    const $ = cheerio.load(data);
    const extensionIconSrc = $('div#section-banner').find('img.image-display').attr().src;
    SetIconSrc(extensionIconSrc);
  };

  useEffect(() => {
    GetIconSrc();
  }, []);

  return (
    <Chip
    component={'a'}
    avatar={(iconSrc && <Avatar alt={id} src={iconSrc} component={'span'}/>) || <CircularProgress size={24} component={'span'}/> }
    label={id.split('.')[1]}
    href={link}
    clickable
    />
  )
}