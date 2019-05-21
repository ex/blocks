
use ggez::*;

struct State
{
    dt: std::time::Duration,
    dx: f32,
}

impl ggez::event::EventHandler for State
{
    fn update( &mut self, ctx: &mut Context ) -> GameResult<()>
    {
        self.dt = timer::delta( ctx );
        Ok(())
    }
    fn draw( &mut self, ctx: &mut Context ) -> GameResult<()>
    {
        ////println!( "dt = {}ns", self.dt.subsec_nanos() );
        graphics::clear( ctx, [0.0, 0.0, 0.0, 1.0].into() );
        let dt = self.dt.subsec_nanos();
        self.dx += dt as f32 * 0.0000001;
        let circle = graphics::Mesh::new_circle(
                ctx,
                graphics::DrawMode::fill(),
                mint::Point2{ x: self.dx, y: 300.0 },
                75.0,
                0.1,
                graphics::Color::new( 1.0, 0.0, 0.0, 1.0 ),
        )?;
        graphics::draw( ctx, &circle, graphics::DrawParam::default() )?;
        graphics::present( ctx )?;
        Ok(())
    }
}

fn main()
{
    let state = &mut State { dt: std::time::Duration::new( 0, 0 ), dx: 0.0 };

    let c = conf::Conf::new();
    let ( ref mut ctx, ref mut event_loop ) = ContextBuilder::new( "Blocks Game", "ex").conf( c ).build().unwrap();

    event::run( ctx, event_loop, state ).unwrap();
}
