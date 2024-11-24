import { Hono } from 'hono'

const app = new Hono()

app.use(async (c, next) => {
  await next();
  c.header("Access-Control-Allow-Origin", "*");
});

app.get('/', (c) => {
  console.log("Root route!");
  return c.text('Hello Hono!', 200);
});

app.post("/upload", async (c) => {
  console.log("Upload route hit!");

  const body = await c.req.arrayBuffer();
  const decoder = new TextDecoder("utf-8");
  const str = decoder.decode(body);
  console.log(str);

  console.log("done.");
  return c.text("Thank you for uploading your file", 200);
});

export default app
