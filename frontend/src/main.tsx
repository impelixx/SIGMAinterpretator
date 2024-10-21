import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'

import './index.module.scss'
import { HelloPage } from './pages/Hello'
import axios from 'axios';


axios.defaults.baseURL = process.env.NODE_ENV === "local" ? "http://localhost:8080" : "https://spotika4.difhel.dev";;

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <HelloPage />
  </StrictMode>
)
